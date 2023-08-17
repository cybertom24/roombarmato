#!/usr/bin/python3

import socket


TCP_IP = "192.168.4.1"
TCP_PORT = 4000
BUFFER_SIZE = 1024

sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sck.bind((TCP_IP, TCP_PORT))
sck.listen(1)

while 1:
    conn, addr = sck.accept()
    print('Connection address:', addr)
    while 1:
        data = conn.recv(BUFFER_SIZE)
        if not data: break
        print("received data:", data)
        conn.send(data)  # echo
    conn.close()
