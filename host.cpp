#include <string>

#include "host.h"


Host::Host(std::string hostname, int port, int maxConn) : m_hostname(hostname), m_port(port), m_maxConn(maxConn), m_online(false), m_busyRatio(0){
}

void Host::setOnline(bool online){
    m_online = online;
}

bool Host::isOnline(){
    return m_online;
}

void Host::increaseBusyRatio(){
    m_busyRatio++;
}

void Host::decreaseBusyRatio(){
    m_busyRatio--;
}

int Host::getBusyRatio(){
    return m_busyRatio;
}

std::string Host::getHostName(){
    return m_hostname;
}

int Host::getPort(){
    return m_port;
}

int Host::getMaxConn(){
    return m_maxConn;
}
