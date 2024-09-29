

import socket
import struct
import time
import threading
import os

TCP_IP = '192.168.123.123'
TCP_PORT = 10000
BUFFER_SIZE = 1024
value = 0
i = 0

if __name__ == "__main__":
 
    while True:

        i = i + 1
        value = "testando" + str(i)
        print(value)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((TCP_IP, TCP_PORT))
        s.send(value.encode())
        s.close()