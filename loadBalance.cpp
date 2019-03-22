#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <vector>
#include <thread>

#include "loadBalance.h"
#include "log.h"
#include "util.h"
#include "algorithmFactory.h"

static const int MAX_EVENT_NUMBER = 10000;
static const int EPOLL_WAIT_TIME = 5000;
static const int BUFF_SIZE = 1024;
static char BUFF[1024];

LoadBalance::LoadBalance(int fd, std::vector<Host*> servers, Base* algorithm, int maxConn) : m_listenFd(fd), m_servers(servers), m_algorithm(algorithm), m_maxConn(maxConn), m_curConn(0) {
    m_epollFd = epoll_create(1024);
    assert(m_epollFd != -1);
    addReadFd(m_epollFd, m_listenFd);
}

LoadBalance::~LoadBalance(){
    for(auto it = m_cltToSrv.cbegin(); it != m_cltToSrv.cend(); ++it){
        closeFd(m_epollFd, it->first);
        closeFd(m_epollFd, it->second);
    }
}


void LoadBalance::balance(){
    epoll_event events[MAX_EVENT_NUMBER];
    int number = 0;
    while(true){
        number = epoll_wait(m_epollFd, events, MAX_EVENT_NUMBER, EPOLL_WAIT_TIME);
        if((number < 0) &&(errno != EINTR)){
            log( LOG_ERR, __FILE__, __LINE__, "%s", "epoll failure" );
            break;
        }

        for(int i = 0; i < number; ++i){
            int sockFd = events[i].data.fd;
            if(sockFd == m_listenFd && events[i].events & EPOLLIN){ //get a new client request
                m_curConn++;
                struct sockaddr_in clientAddress;
                socklen_t clientAddrlength = sizeof(clientAddress);
                int cltFd = accept(m_listenFd, (struct sockaddr*)&clientAddress, &clientAddrlength);
                if(cltFd < 0){
                    log(LOG_ERR, __FILE__, __LINE__, "Accept client request fail, errno: %s", strerror(errno));
                    continue;
                }
                if(m_curConn > m_maxConn){ //到达最大连接数则拒绝客户端请求
                    log(LOG_DEBUG, __FILE__, __LINE__, "%s", "Max connection reached! The request from client refused!");
                    close(cltFd);
                    continue;
                }
                Host* server = m_algorithm->selectServer(); 
                if(server->getBusyRatio() >= server->getMaxConn()){
                    log(LOG_ERR, __FILE__, __LINE__, "server %s has reached the maximum number of connections!", (char*)server->getHostName().c_str());
                    continue;
                }
         
                struct sockaddr_in serverAddress;
                bzero(&serverAddress, sizeof(serverAddress));
                serverAddress.sin_family = AF_INET;
                char* hostName = (char*)server->getHostName().c_str();
                inet_pton(AF_INET, hostName, &serverAddress.sin_addr);
                serverAddress.sin_port = htons(server->getPort());
                int srvFd = socket(PF_INET, SOCK_STREAM, 0);
                if(srvFd < 0){
                    log(LOG_ERR, __FILE__, __LINE__, "%s", "Create socket fail!");
                    continue;
                }
                if(connect(srvFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0){
                    log(LOG_ERR, __FILE__, __LINE__, "Connect to host %s fail!", hostName);
                    closeFd(m_epollFd, srvFd);
                    continue;
                }
                server->increaseBusyRatio(); 
                addReadFd(m_epollFd, cltFd);
                addReadFd(m_epollFd, srvFd);
                m_cltToSrv[cltFd] = srvFd;
                m_srvToClt[srvFd] = cltFd;
                m_srvFdToSrv[srvFd] = server;
            }else if(m_cltToSrv.count(sockFd) > 0  && events[i].events & EPOLLIN){ //This is a client socket
                int srvFd = m_cltToSrv[sockFd];
                int bytesRead = recv(sockFd, BUFF, BUFF_SIZE, 0); //receive from client
                if(bytesRead == -1){
                    if(errno == EAGAIN || errno == EWOULDBLOCK){
                        continue;
                    }else{
                        log(LOG_ERR, __FILE__, __LINE__, "Receive from client met error: %s", strerror(errno));
                        freeConn(sockFd, srvFd);
                        continue;
                    }
                }
                if(bytesRead == 0){
                    freeConn(sockFd, srvFd);
                    continue;
                }
                if(send(srvFd, BUFF, bytesRead, 0) < 0){  //send to server
                    freeConn(sockFd, srvFd);
                    continue;
                }
            }else{
                int cltFd = m_srvToClt[sockFd];
                int bytesRead = recv(sockFd, BUFF, BUFF_SIZE, 0); //receive from client
                if(bytesRead == -1){
                    if(errno == EAGAIN && errno == EWOULDBLOCK){
                        continue;
                    }else{
                        log(LOG_ERR, __FILE__, __LINE__, "Receive from server met error: %s", strerror(errno));
                        freeConn(cltFd, sockFd);
                        continue;
                    }
                }                                                                                                                               if(bytesRead == 0){
                    freeConn(cltFd, sockFd);
                    continue;
                }
                if(send(cltFd, BUFF, bytesRead, 0) < 0){  //send to client
                    freeConn(cltFd, sockFd);
                    continue;
                }
            }
        }
        
    }
}

void LoadBalance::freeConn(int cltFd, int srvFd){
    closeFd(m_epollFd, cltFd);
    closeFd(m_epollFd, srvFd);
    m_cltToSrv.erase(cltFd);
    m_srvToClt.erase(srvFd);
    m_srvFdToSrv[srvFd]->decreaseBusyRatio();
    m_curConn--;
}

