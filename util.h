#ifndef UTIL_H
#define UTIL_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

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

#endif
