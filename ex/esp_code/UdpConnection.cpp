#include "UdpConnection.h"
#include "Command.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

UdpConnection::UdpConnection(String mySsid, String myPassword, int myPort) {
  MYSSID = mySsid;
  MYPASSWORD = myPassword;
  MYPORT = myPort;
}

WiFiUDP _udp;

void UdpConnection::setup() {
  Serial.begin(74880);
  if(debug) {
    Serial.println("\n");
    Serial.println("Booting up...");
  }
  // Creazione dell'AP (togliendo il parametro PW l'AP diventa pubblico)
  WiFi.softAP(MYSSID, MYPASSWORD);
  if (debug) {
    Serial.printf("AP created!\n\t> SSID: %s\n", MYSSID.c_str());
    Serial.printf("\t> PW: %s\n", MYPASSWORD.c_str());
  }
  // Creazione del socket UDP
  _udp.begin(MYPORT);
  if (debug) {
    Serial.printf("UDP socket open!\n");
    Serial.printf("\t> IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("\t> Port: %d\n", MYPORT);
  }
}

int UdpConnection::checkPackets() {
  receivedPacketSize = _udp.parsePacket();
  if (receivedPacketSize > 0) {
    // Salva l'indirizzo
    senderIP = _udp.remoteIP().toString();
    senderPort = _udp.remotePort();
    // Leggi il pacchetto e buttalo nel buffer
    free(receivedPacket);
    receivedPacket = (byte*) malloc(sizeof(byte) * receivedPacketSize);
    _udp.read(receivedPacket, receivedPacketSize);
    
    if(debug) {
      Serial.printf("Received packet from %s:%d containing:\n", senderIP.c_str(), senderPort);
      Serial.printf("\t%s\n", receivedPacket);
    }
  }
}

int UdpConnection::getPacketSize() {
  return receivedPacketSize;
}

byte UdpConnection::getPacket(byte *array) {
  memcpy(array, receivedPacket, receivedPacketSize);
  return 0;
}

/*
void UdpConnection::send(byte *message, int length) {
  char _buffer[COMMAND_SIZE];
  for(int i = 0; i < COMMAND_SIZE; i++) {
    _buffer[i] = message[i];
  }
  _udp.beginPacket(senderIP.c_str(), senderPort);
  _udp.write(_buffer);
  _udp.endPacket();
}
*/