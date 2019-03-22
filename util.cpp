#include "util.h"

int setnonblocking(int fd){ 
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addReadFd(int epollFd, int fd){ 
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void closeFd(int epollFd, int fd){ 
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

int connectToServer(char* hostName, int port){ 
    struct sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, hostName, &serverAddress.sin_addr);
    serverAddress.sin_port = htons(port);
    int srvFd = socket(PF_INET, SOCK_STREAM, 0);
    if(srvFd < 0){
        log(LOG_ERR, __FILE__, __LINE__, "%s", "Create socket fail!");
        return -1;
    }
    if(connect(srvFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0){
        log(LOG_ERR, __FILE__, __LINE__, "Connect to host %s fail!", hostName);
        close(srvFd);
        return -1;
    }
    
    return srvFd; 
}