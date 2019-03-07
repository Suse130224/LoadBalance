#!/usr/bin/python
from socket import *
import ConfigParser
import time
import thread

def check_server(address, port):
    sock = socket(AF_INET, SOCK_STREAM)
    try:
        sock.connect((address, port))
        sock.send("Hello")
        if(sock.recv(8192) == "Hello"):
            return True
        else:
            return False
    except error, e:
        print("exception occur: {}").format(e)
        return False
        

if __name__ == '__main__':
    cf = ConfigParser.ConfigParser()
    cf.read("test.conf")
    hostname = cf.get("host", "hostname")
    port = cf.getint("host", "port")
    print("hostname: {}, port: {}").format(hostname, port)
    testInterval = cf.getfloat("test", "interval")
    testTime = cf.getint("test", "time")
    startTime = time.time()

    total = 0
    success = 0
    while(time.time() - startTime < testTime):
        time.sleep(testInterval)
        for i in range(100):
            thread.start_new_thread(check_server, (hostname, port))
        

    print("Total :{}, success: {}").format(total, success)
