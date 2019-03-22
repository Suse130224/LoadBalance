#ifndef ALGORITHMFACTORY_H
#define ALGORITHMFACTORY_H

#include <cstdlib>
#include <climits>
#include <vector>
#include <string>
#include "host.h"
#include "log.h"

const std::string ROUND_ROBIN = "roundRobin";
const std::string RANDOM = "random";
const std::string LEAST_CONNECTION = "leastConnection";

class Base{
public:
	Base(std::vector<Host* > servers) : m_servers(servers) {}
	virtual Host* selectServer() = 0;
protected:
	std::vector<Host* > m_servers;
};

class RoundRobin : public Base{
public:
	static int pos;
	RoundRobin(std::vector<Host* > servers) : Base(servers) {}
	Host* selectServer(){
		if(pos > m_servers.size()){
			pos = 0;
		}
		Host* server = m_servers[pos];
		pos++;
		return server;
	}
};


class Random : public Base{
public:
	Random(std::vector<Host* > servers) : Base(servers) {}
	Host* selectServer(){
		int randomPos = rand() % m_servers.size();
		return m_servers[randomPos];
	}
};

class LeastConnection : public Base{
public:
	LeastConnection(std::vector<Host* > servers) : Base(servers) {}
	Host* selectServer(){
		int minBusyRatio = INT_MAX;
    	Host* res = nullptr;
    	for(auto server : m_servers){
        	if(server->getBusyRatio() < minBusyRatio){
            	res = server;
            	minBusyRatio = server->getBusyRatio();
        	}
    	}
    	return res;
	}
};

class AlgorithmFactory{
public:
	AlgorithmFactory(std::string name, std::vector<Host* > servers) : m_name(name), m_servers(servers) {}
	Base* create(){
		if(m_name == ROUND_ROBIN){
			return new RoundRobin(m_servers);
		}
		else if(m_name == RANDOM){
			return new Random(m_servers);
		}
		else if(m_name == LEAST_CONNECTION){
			return new LeastConnection(m_servers);
		}
		else{
			log( LOG_ERR, __FILE__, __LINE__, "Currenly only support %s, %s, and %s!", ROUND_ROBIN, RANDOM, LEAST_CONNECTION );
			return nullptr;
		}
	}
private:
	std::string m_name;
	std::vector<Host* > m_servers;
};

#endif