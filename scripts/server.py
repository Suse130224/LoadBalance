#!/usr/bin/python

from socket import socket, AF_INET, SOCK_STREAM

def echo_handler(address, client_sock):
    while True:
        msg = client_sock.recv(8192)
        print('Received message: {}'.format(msg))
        if not msg:
            break
        client_sock.sendall(msg)
    client_sock.close()
       
def echo_server(address, backlog=5):
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind(address)
    sock.listen(backlog)
    while True:
        client_sock, client_addr = sock.accept()
        echo_handler(client_addr, client_sock)
                                                                  
if __name__ == '__main__':
    from optparse import OptionParser
    parser=OptionParser()
    parser.add_option("-p", "--port", dest="port", type="int", default=80, help="PORT for server", metavar="PORT")
    (options,args)=parser.parse_args()
    echo_server(('', options.port))
