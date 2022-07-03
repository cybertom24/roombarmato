#ifndef UdpConnection_H
  #define UdpConnection_H
  // Librerie
  #include "arduino.h"
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include "Command.h"
  #include "CustomBuffer.h"
  
  class UdpConnection {
    public:
      UdpConnection(String mySsid, String myPassword, int myPort);      
      void setup();
      void checkPackets();
      void refresh();
      CustomBuffer getMessage();
      void send(byte message[Command::SIZE]);
      bool debug = false, newMessage = false;
    private:
      String MYSSID, MYPASSWORD;
      int MYPORT;
      String senderIP;
      int senderPort;
      WiFiUDP _udp;
      byte receivedBuffer[Command::SIZE]; // Buffer per tenere i pacchetti ricevuti
  };
#endif
