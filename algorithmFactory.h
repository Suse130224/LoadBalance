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

class Base{ //虚基类，提供接口
public:
	Base(std::vector<Host* > servers);
	virtual Host* selectServer() = 0;
protected:
	std::vector<Host* > m_servers;
};

class RoundRobin : public Base{ //轮询法
public:
	RoundRobin(std::vector<Host* > servers);
	Host* selectServer();
private:
	static int pos;
};


class Random : public Base{ //随机法
public:
	Random(std::vector<Host* > servers);
	Host* selectServer();
};

class LeastConnection : public Base{ //最小连接数法
public:
	LeastConnection(std::vector<Host* > servers);
	Host* selectServer();
};

class AlgorithmFactory{ //算法工厂，根据配置创建对应的对象
public:
	AlgorithmFactory(std::string name, std::vector<Host* > servers);
	Base* create();
private:
	std::string m_name;
	std::vector<Host* > m_servers;
};

#endif