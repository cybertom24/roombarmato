#ifndef UdpConnection_H
  #define UdpConnection_H
  // Librerie
  #include "arduino.h"
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include "Command.h"
  
  class UdpConnection {
    public:
      UdpConnection(String mySsid, String myPassword, int myPort);   

      // Inizializza la connessione
      void setup();

      // Ritorna la lunghezza del pacchetto arrivato (se la lunghezza è -1 vuol dire che non c'è nessun pacchetto)
      int checkPackets();

      // Copia il messaggio che è arrivato nello spazio di memoria indicato dal puntatore passato come argomento
      // ATTENZIONE: Il puntatore array deve già puntare a della memoria sufficientemente grande
      // La funzione ritorna 0 se è andata a buon fine, -1 in caso ci siano stati errori
      byte getPacket(byte *array);
      int getPacketSize();

      // void send(byte *message, int length);
      bool debug = false;
    
    private:
      String MYSSID, MYPASSWORD;
      int MYPORT;
      String senderIP;
      int senderPort;
      WiFiUDP _udp;
      byte *receivedPacket; // Buffer per tenere i pacchetti ricevuti
      int receivedPacketSize;
  };
#endif
