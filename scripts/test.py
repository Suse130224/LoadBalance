#!/usr/bin/python
from socket import *
import ConfigParser
import time
import threading
from optparse import OptionParser

total = 0
success = 0

def checkServer(address, port):
    sock = socket(AF_INET, SOCK_STREAM)
    global total
    global success
    try:
        sock.settimeout(3)
        sock.connect((address, port))
        sock.send("Hello")
        total += 1
        if(sock.recv(8192) == "Hello"):
            success += 1
    except error, e:
        print("exception occur: {}").format(e)
    sock.close()

if __name__ == '__main__':
    cf = ConfigParser.ConfigParser()
    cf.read("test.conf")
    hostname = cf.get("host", "hostname")
    port = cf.getint("host", "port")
    print("hostname: {}, port: {}").format(hostname, port)
    

    parser=OptionParser()
    parser.add_option("-c", "--concurrency", dest="concurrency", type="int", default=80)
    (options,args)=parser.parse_args()
    concurrency = options.concurrency

    startTime = time.time()
    threads = []
    for i in xrange(concurrency):
        t = threading.Thread(target=checkServer, args=(hostname, port))
        threads.append(t)
    for i in xrange(concurrency):
        threads[i].start()
    for i in xrange(concurrency):
        threads[i].join()
        
    print("Test comleted! Total: {}, success: {}, ratio: {}, total time: {}").format(total, success, float(success)/ total, time.time() - startTime)
