#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>

#include "log.h"

int setnonblocking(int fd); //将socket设置为非阻塞模式

void addReadFd(int epollFd, int fd); //将fd添加到epollFd的监听列表

void closeFd(int epollFd, int fd); //将fd从epollFd的监听列表移除，并关闭fd

int connectToServer(char* hostName, int port); //创建socket并连接到服务器

#endif
