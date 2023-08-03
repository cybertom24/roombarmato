#ifndef TcpConnection_H
#define TcpConnection_H
// Librerie
#include "Arduino.h"
#include <ESP8266WiFi.h>

#ifndef SERIAL_BAUD_RATE
    #define SERIAL_BAUD_RATE 115200
#endif

#ifndef MAX_BUFFER_SIZE
    #define MAX_BUFFER_SIZE 256
#endif

class TcpConnection
{
public:
    TcpConnection(String mySsid, String myPassword, int myPort);

    // Inizializza la connessione
    void setup();

    // Controlla che si sia collegato qualcuno. Attenzione: la funzione blocca l'esecuzione finché non si collega qualcuno a meno che non scada il timeout
    // Per disattivare il timeout impostarlo a zero. Esprimere il timeout in microsecondi
    boolean waitClient(size_t timeout);

    // Ritorna se c'è un client connesso
    boolean clientConnected();

    // Ritorna il numero di byte disponibili da leggere
    size_t available();

    // Copia il messaggio che è arrivato nello spazio di memoria indicato dal puntatore passato come argomento
    // ATTENZIONE: Il puntatore array deve già puntare a della memoria sufficientemente grande
    // Ritorna il numero di bytes trascritti
    size_t readBytes(uint8_t *array, size_t num);

    // Invia un messaggio all'ultimo dispositivo che ha comunicato. Ritorna true se l'invio è andato a buon fine,
    // altrimenti ritorna false
    boolean send(uint8_t *message, int length);
    boolean debug = false;

private:
    String MYSSID, MYPASSWORD;
    int MYPORT;
    WiFiServer *tcpServer;
    WiFiClient client;
};
#endif
