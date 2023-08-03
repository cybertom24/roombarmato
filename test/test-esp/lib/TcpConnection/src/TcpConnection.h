#ifndef TcpConnection_H
#define TcpConnection_H
// Librerie
#include "Arduino.h"
#include <ESP8266WiFi.h>

#ifndef SERIAL_BAUD_RATE
    #define SERIAL_BAUD_RATE 115200
    #define MAX_PACKET_SIZE  256
#endif

class TcpConnection
{
public:
    TcpConnection(String mySsid, String myPassword, int myPort);

    // Inizializza la connessione
    void setup();

    // Controlla che si sia collegato qualcuno. Attenzione: la funzione blocca l'esecuzione finché non si collega qualcuno a meno che non scada il timeout
    // Per disattivare il timeout impostarlo a zero. Esprimere il timeout in microsecondi
    boolean waitClient(unsigned long timeout);

    // Ritorna se c'è un client connesso
    boolean clientConnected();

    // Ritorna la lunghezza del pacchetto arrivato (se la lunghezza è -1 vuol dire che non c'è nessun pacchetto)
    int checkPackets();

    // Copia il messaggio che è arrivato nello spazio di memoria indicato dal puntatore passato come argomento
    // ATTENZIONE: Il puntatore array deve già puntare a della memoria sufficientemente grande
    // La funzione ritorna 0 se è andata a buon fine, -1 in caso ci siano stati errori
    uint8_t getPacket(uint8_t *array);

    // Ritorna la dimensione in byte dell'ultimo paccchetto arrivato e che è al momento contenuto nel buffer
    int getPacketSize();

    // Invia un messaggio all'ultimo dispositivo che ha comunicato. Ritorna true se l'invio è andato a buon fine,
    // altrimenti ritorna false
    boolean send(uint8_t *message, int length);
    bool debug = false;

private:
    String MYSSID, MYPASSWORD;
    int MYPORT;
    WiFiServer *tcpServer;
    WiFiClient client;
    String senderIP;
    int senderPort;
    uint8_t bufferIn[MAX_PACKET_SIZE]; // Buffer per tenere i pacchetti ricevuti
    int receivedPacketSize;
};
#endif
