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

HealthCheck::HealthCheck(std::vector<Host> servers) : m_servers(servers){
}

void HealthCheck::start(){
    std::thread t(&HealthCheck::check, this);
    t.join();
}

void HealthCheck::check() {
    std::map<std::string, int> numOfFail;
    std::map<std::string, int> numOfSuccess;
    for(auto server : m_servers){
        numOfFail[server.getHostName()] = 0;
        numOfSuccess[server.getHostName()] = 0;
    }
    while(true){
        sleep(SEND_TIME_INTERVAL);
        for(auto it = m_servers.begin(); it != m_servers.end(); ++it){
            std::string hostname = (*it).getHostName();
            struct sockaddr_in address;
            bzero(&address, sizeof(address));
            address.sin_family = AF_INET;
            inet_pton(AF_INET, (char*)hostname.c_str(), &address.sin_addr);
            address.sin_port = htons((*it).getPort());

            int sockFd = socket(PF_INET, SOCK_STREAM, 0);
            assert(sockFd > 0);
            if(connect(sockFd, (struct sockaddr*)&address, sizeof(address)) != 0){
                numOfFail[hostname]++;
                numOfSuccess[hostname] = 0;
            }else{
                numOfFail[hostname] = 0;
                numOfSuccess[hostname]++;
            }
            if(numOfFail[hostname] > THRESHOLD && (*it).isOnline()){ //After several failed connections, the server is considered not online                     
                log(LOG_ERR, __FILE__, __LINE__, "Host %s not online",(char*)hostname.c_str());
                (*it).setOnline(false);
            }
            if(numOfSuccess[hostname] > THRESHOLD && !(*it).isOnline()){ //After serveral successful connections, the server is considered to be restored
                log(LOG_INFO, __FILE__, __LINE__, "Host %s has resumed service", (char*)hostname.c_str());
                (*it).setOnline(true);
            }
            close(sockFd);
        }
    }
}
