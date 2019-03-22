#ifndef LOADBALANCE_H
#define LOADBALANCE_H

#include <vector>
#include <map>

#include "host.h"
#include "algorithmFactory.h"

class LoadBalance{
private:
    int m_listenFd;
    int m_epollFd;
    int m_maxConn;
    int m_curConn;
    std::vector<Host*> m_servers;
    std::map<int, int> m_cltToSrv; //用于保存客户端fd对应的服务端fd，方便转发
    std::map<int, int> m_srvToClt; //用于保存服务端fd对应的客户端fd，方便转发
    std::map<int, Host*> m_srvFdToSrv; //用于保存服务端fd对应的server指针，用于释放连接时让对应Host对象的busyRatio减一
    Base* m_algorithm; //负载均衡器采取的负载均衡算法
    void freeConn(int cltFd, int srvFd); //释放连接。将客户端和服务端的fd从epoll列表中删除，close二者，并将当前服务器的busyRatio和负载均衡器的客户端连接数减一
    void sendToServer(int sockFd); //将客户端的数据转发给服务端
    void sendToClient(int sockFd); //将服务端的数据转发给客户端
public:
    LoadBalance(int fd, std::vector<Host*> servers, Base* algorithm, int maxConn);
    ~LoadBalance();
    void balance();
    void check();
};

#endif
