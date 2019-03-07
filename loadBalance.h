#ifndef LOADBALANCE_H
#define LOADBALANCE_H

#include <vector>
#include <map>

#include "host.h"

class LoadBalance{
private:
    int m_listenFd;
    int m_epollFd;
    std::vector<Host*> m_servers;
    std::map<int, int> m_cltToSrv;
    std::map<int, int> m_srvToClt;
    std::map<int, Host*> m_srvFdToSrv;
    void freeConn(int cltFd, int srvFd);
public:
    LoadBalance(int fd, std::vector<Host*> servers);
    ~LoadBalance();
    Host* getMostFreeHost();
    void balance();
    void check();
};

#endif
