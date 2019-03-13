#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <map>
#include <thread>

#include "healthCheck.h"
#include "log.h"


HealthCheck::HealthCheck(std::vector<Host*> servers, int inter, int rise, int fall) : m_servers(servers), m_inter(inter), m_rise(rise), m_fall(fall){
}


void HealthCheck::check(){
    std::map<std::string, int> numOfFail;
    std::map<std::string, int> numOfSuccess;
    for(auto server : m_servers){
        numOfFail[server->getHostName()] = 0;
        numOfSuccess[server->getHostName()] = 0;
    }
    while(true){
        sleep(m_inter);
        for(auto server : m_servers){
            std::string hostname = server->getHostName();
            struct sockaddr_in address;
            bzero(&address, sizeof(address));
            address.sin_family = AF_INET;
            inet_pton(AF_INET, (char*)hostname.c_str(), &address.sin_addr);
            address.sin_port = htons(server->getPort());

            int sockFd = socket(PF_INET, SOCK_STREAM, 0);
            assert(sockFd > 0);
            if(connect(sockFd, (struct sockaddr*)&address, sizeof(address)) != 0){
                if(numOfFail[hostname] < m_fall){
                    numOfFail[hostname]++;
                }
                numOfSuccess[hostname] = 0;
            }else{
                if(numOfSuccess[hostname] < m_rise){
                    numOfSuccess[hostname]++;
                }
                numOfFail[hostname] = 0;
            }
            close(sockFd);
            if(numOfFail[hostname] >= m_fall && server->isOnline()){ //After several failed connections, the server is considered not online                     
                log(LOG_ERR, __FILE__, __LINE__, "Host %s not online", hostname.c_str());
                server->setOnline(false);
            }
            if(numOfSuccess[hostname] >= m_rise && !server->isOnline()){ //After serveral successful connections, the server is considered to be restored
                log(LOG_INFO, __FILE__, __LINE__, "Host %s has resumed service", (char*)hostname.c_str());
                server->setOnline(true);
            }
        }
    }
}
