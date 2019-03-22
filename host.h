#ifndef HOST_H
#define HOST_H

#include <string>
#include <atomic>

class Host
{
private:
    std::string m_hostname;
    int m_port;
    int m_maxConn; //服务器支持的最大连接数
    std::atomic<bool> m_online; //根据健康检查结果设置是否在线
    int m_busyRatio; //服务器当前连接数
public:
    Host(std::string hostname, int port, int maxConn);
    void setOnline(bool online);
    bool isOnline();
    void increaseBusyRatio();
    void decreaseBusyRatio();
    int getBusyRatio();
    std::string getHostName();
    int getPort();
    int getMaxConn();
};

#endif
