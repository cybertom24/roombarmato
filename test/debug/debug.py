#!/usr/bin/python3
import socket
import command
import threading
from time import sleep
from datetime import datetime as dt

# Costanti
ADDRESS = "192.168.4.1"
PORT = 4000
BUFFER_SIZE = 1024
millis = 100
duration = 100
answered = False
measuring_max_freq = False

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
    global answered, measuring_max_freq
    while True:
        data = sck.recv(BUFFER_SIZE)
        if not measuring_max_freq:
            print("\n> Ricevuto:", list(data))
        answered = True

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

def measure_max_freq(tries, sck):
    global answered, measuring_max_freq
    measuring_max_freq = True
    print("measuring max delay between messages. Tries:", tries)
    delay_tot = 0
    i = tries
    while i > 0:
        print(int(100 * (tries - i) / tries))
        # Imposta il flag a falso
        answered = False
        # Leggi il tempo corrente
        start_time = dt.now()
        # Invia un comando di CHECK_CONNECTION
        send_command((command.START_BYTE, 0x16, 0x00, 0x00, 0x00, command.END_BYTE), sck)
        # aspetta la risposta
        while not answered:
            # Non fare nulla
            pass
        # Calcola il delay
        delay = dt.now() - start_time
        delay_us = int(delay.total_seconds() * 1_000_000)
        # Aggiungilo al totale
        delay_tot += delay_us
        i -= 1
    # Calcola la media
    delay_m = delay_tot / tries
    print("test finito")
    print("delay medio:", delay_m, "us")
    # Calcola la frequenza: visto che la media è in us, moltiplico per 1000 in modo da ottenere la freq in kHz
    freq = 1_000 / float(delay_m)
    print("freq:", freq, " kHz")
    measuring_max_freq = False


# ----------------------------------------------------------------
# Inizio script:

sck =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sck.connect((ADDRESS, PORT))


listen_thread = threading.Thread(target=listen, args=(sck,))
listen_thread.daemon = True
listen_thread.start()

sleep(2)
measure_max_freq(1000, sck)

while False:
    # sck.send(bytes([0, 0]))
    # spam_test()
    newCommand = get_command_from_console()
    print(newCommand)
    send_command(newCommand, sck)
    # print("> new command\ncode =", newCommand[], "\ndata0 =", data0, "\ndata1 =", data1)

# Chiusura
sck.close()

