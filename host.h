#ifndef HOST_H
#define HOST_H

#include <string>

class Host
{
private:
    std::string m_hostname;
    int m_port;
    int m_maxConn; //Maximum number of connections supported by the server
    bool m_online; //Set by the result of health check
    int m_busyRatio; //Number of connection
public:
    Host(std::string hostname, int port, int maxConn);
    void setOnline(bool online);
    bool isOnline();
    void setBusyRatio(int busyRatio);
    int getBusyRatio();
    std::string getHostName();
    int getPort();
    int getMaxConn();
};

#endif
