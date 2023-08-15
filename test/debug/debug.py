#!/usr/bin/python3
import socket
import command
import threading
from time import sleep

# Costanti
ADDRESS = "192.168.4.1"
PORT = 4000
BUFFER_SIZE = 1024
millis = 100
duration = 100

# Funzioni
def get_command_from_console():
    message = input("code, data0, data1: ")
    list = message.split(",")
    # print(list)
    if len(list) == 0:
        return None
    code = int(list[0])
    data0 = 0
    data1 = 0
    size = 0
    if len(list) == 3:
        data0 = int(list[1])
        data1 = int(list[2])
        size = 2
    if len(list) == 2:
        data0 = int(list[1])
        size = 1
    return (command.START_BYTE, code, size, data0, data1, command.END_BYTE)

def listen(sck):
    while True:
        data = sck.recv(BUFFER_SIZE)
        print("\n> Ricevuto:", list(data))

def send_command(command, sck):
    sck.send(bytes(command))

def spam_test():
    # Proviamo a riempire l'holder
    print("spam test:", millis, " ms between messages")
    i = duration
    while i > 0:
        send_command((command.START_BYTE, 9, 2, i, 100, command.END_BYTE), sck)
        # send_command((0x69, 0x09, 0x02, 0xC2, 0xC2, 0xFA), sck)
        i -= 1
        sleep(millis / 1000)


# ----------------------------------------------------------------
# Inizio script:

sck =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sck.connect((ADDRESS, PORT))


listen_thread = threading.Thread(target=listen, args=(sck,))
listen_thread.daemon = True
listen_thread.start()

while 1:
    # spam_test()
    newCommand = get_command_from_console()
    print(newCommand)
    send_command(newCommand, sck)
    # print("> new command\ncode =", newCommand[], "\ndata0 =", data0, "\ndata1 =", data1)

# Chiusura
sck.close()

