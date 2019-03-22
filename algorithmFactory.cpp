#include "algorithmFactory.h"

int RoundRobin::pos = 0;

Base::Base(std::vector<Host* > servers) : m_servers(servers) {}

RoundRobin::RoundRobin(std::vector<Host* > servers) : Base(servers) {}

Host* RoundRobin::selectServer(){
	if(pos >= m_servers.size()){
		pos = 0;
	}
	Host* server = m_servers[pos];
	pos++;
	return server;
}

Random::Random(std::vector<Host* > servers) : Base(servers) {}
	
Host* Random::selectServer(){
	int randomPos = rand() % m_servers.size();
	return m_servers[randomPos];
}

LeastConnection::LeastConnection(std::vector<Host* > servers) : Base(servers) {}

Host* LeastConnection::selectServer(){
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

AlgorithmFactory::AlgorithmFactory(std::string name, std::vector<Host* > servers) : m_name(name), m_servers(servers) {}

Base* AlgorithmFactory::create(){
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