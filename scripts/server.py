#!/usr/bin/python
# -*- coding: utf-8 -*-

import socket
import select
import time
from optparse import OptionParser

# 读取选项
parser=OptionParser()
parser.add_option("-p", "--port", dest="port", type="int", default=80, help="PORT for server", metavar="PORT")
(options,args)=parser.parse_args()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 可重复绑定
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

s.bind(("", options.port))
s.listen(1024)

epoll = select.epoll()

# 在epoll中注册s套接字
epoll.register(s.fileno(), select.EPOLLIN|select.EPOLLET)


# 创建两个字典, 来保存fileno和与其对应的套接字和地址
connections = {}
addresses = {}

while True:
    epollList = epoll.poll()
    for fd,events in epollList:
        if fd == s.fileno():
            conn,addr = s.accept()
            connections[conn.fileno()] = conn
            addresses[conn.fileno()] = addr

            epoll.register(conn.fileno(), select.EPOLLIN|select.EPOLLET)

        # 如果是接收到了数据
        elif events == select.EPOLLIN:
            recvData = connections[fd].recv(1024)
            if len(recvData) > 0:
                print("{} received data: {}").format(time.time(), recvData)
                connections[fd].send(recvData)
            else:
                epoll.unregister(fd)
                connections[fd].close()
