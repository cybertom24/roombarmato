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
  // Creazione dell'AP (togliendo il parametro PW l'AP diventa pubblico
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

void UdpConnection::checkPackets() {
  int packetSize = _udp.parsePacket();
  if (packetSize > 0) {
    newMessage = true;
    // Salva l'indirizzo
    senderIP = _udp.remoteIP().toString();
    senderPort = _udp.remotePort();
    // Leggi il pacchetto e buttalo nel buffer
    _udp.read(receivedBuffer, packetSize);
    
    if(debug) {
      Serial.printf("Received packet from %s:%d containing:\n", senderIP.c_str(), senderPort);
      Serial.printf("\t%s\n", receivedBuffer);
    }
  }
}

void UdpConnection::refresh() {
  newMessage = false;
}

CustomBuffer UdpConnection::getMessage() {
  return CustomBuffer(receivedBuffer);
}

void UdpConnection::send(byte message[Command::SIZE]) {
  char _buffer[Command::SIZE];
  for(int i = 0; i < Command::SIZE; i++) {
    _buffer[i] = message[i];
  }
  _udp.beginPacket(senderIP.c_str(), senderPort);
  _udp.write(_buffer);
  _udp.endPacket();
}
