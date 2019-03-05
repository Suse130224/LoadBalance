#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "log.h"
//#include "host.h"
#include "loadBalance.h"
#include "healthCheck.h"

static const char* CONFIG_FILE_PATH = "config";

int main(){

    std::ifstream in(CONFIG_FILE_PATH); //read config file
    if(! in.is_open()){
        log( LOG_ERR, __FILE__, __LINE__, "%s", "open config file fail");
        return 1;
    }
    std::vector<Host> logicalSrvs;
    std::string hostName;
    int port;
    int conns;
    in >> hostName >> port;
    std::string localHostname = hostName;
    int localPort = port;
    while(in >> hostName >> port >> conns){
        Host tmpHost(hostName, port, conns);
        logicalSrvs.push_back(tmpHost);
    }
    in.close();
    
    int listenFd = socket(PF_INET, SOCK_STREAM, 0); //start listening
    assert(listenFd >= 0);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    char* ip = (char*)localHostname.c_str();
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(localPort);
    int ret = bind(listenFd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenFd, 5);
    assert(ret != -1);
    

    LoadBalance loadBalance(listenFd, logicalSrvs);
    HealthCheck healthCheck(logicalSrvs);
    loadBalance.start(); //open a thread for load balance
    healthCheck.start(); // open a thread for health check

    return 0;
}

