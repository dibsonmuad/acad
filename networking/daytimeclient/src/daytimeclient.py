#!/usr/bin/python
import socket
if __name__=='__main__':
    import sys
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if 2 != len(sys.argv):
        print("usage: {} <hostname>".format(sys.argv[0]))
        sys.exit(-1)
    host = sys.argv[1]
    port = 13
    s.connect((host,port))
    data = bytearray()
    while True:
        recv_bytes = s.recv(100)
        if recv_bytes:
            data.extend(recv_bytes)
        else:
            break

    print(data)

    s.close()
