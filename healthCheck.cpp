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
#include "util.h"

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
            int sockFd = connectToServer((char*)hostname.c_str(), server->getPort());
            if(sockFd < 0){ //如果连接失败则增加失败计数
                if(numOfFail[hostname] < m_fall){
                    numOfFail[hostname]++;
                }
                numOfSuccess[hostname] = 0;
            }else{
                if(numOfSuccess[hostname] < m_rise){ //如果连接成功则增加成功计数
                    numOfSuccess[hostname]++;
                }
                numOfFail[hostname] = 0;
            }
            close(sockFd);
            if(numOfFail[hostname] >= m_fall && server->isOnline()){ //如果失败次数超过阈值，且当前状态为online，则将server的online设置为false                     
                log(LOG_ERR, __FILE__, __LINE__, "Host %s not online", hostname.c_str());
                server->setOnline(false);
            }
            if(numOfSuccess[hostname] >= m_rise && !server->isOnline()){ //如果成功次数超过阈值，且当前状态为not online，则将server的online设置为true
                log(LOG_INFO, __FILE__, __LINE__, "Host %s has resumed service", (char*)hostname.c_str());
                server->setOnline(true);
            }
        }
    }
}
