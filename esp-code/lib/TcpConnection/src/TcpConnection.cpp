#include "TcpConnection.h"
#include "Command.h"
#include <ESP8266WiFi.h>

TcpConnection::TcpConnection(String mySsid, String myPassword, int myPort) {
    MYSSID = mySsid;
    MYPASSWORD = myPassword;
    MYPORT = myPort;
}

void TcpConnection::setup()
{
    Serial.begin(74880);
    if (debug)
    {
        Serial.println("\n");
        Serial.println("Booting up...");
    }
    // Creazione dell'AP (togliendo il parametro PW l'AP diventa pubblico)
    WiFi.softAP(MYSSID, MYPASSWORD);
    if (debug)
    {
        Serial.printf("AP created!\n\t> SSID: %s\n", MYSSID.c_str());
        Serial.printf("\t> PW: %s\n", MYPASSWORD.c_str());
    }
    // Creazione del server TCP
    tcpServer = new WiFiServer((uint16_t) MYPORT);
    tcpServer->begin();
    
    if (debug)
    {
        Serial.printf("TCP socket open!\n");
        Serial.printf("\t> IP: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("\t> Port: %d\n", MYPORT);
    }
}

boolean TcpConnection::waitClient(unsigned long timeout) {
    unsigned long startMicroseconds = micros();
    
    do {
        client = tcpServer->available();
    } while (!client && startMicroseconds + timeout < micros());

    if(client)
        return true;
    else
        return false;
}

boolean TcpConnection::clientConnected() {
    if(!client)
        return false;
    
    return client.connected();
}

int TcpConnection::checkPackets()
{
    if(!clientConnected())
        return -1;
    
    if(client.available() == 0)
        return -1;
    
    receivedPacketSize = client.available();
    free(receivedPacket);
    receivedPacket = (byte *) malloc(receivedPacketSize * sizeof(byte));
    client.read(receivedPacket, receivedPacketSize);

    if (debug) {
            Serial.printf("Received packet from %s:%d of lenght: %d\n", client.remoteIP().toString().c_str(), client.remotePort(), receivedPacketSize);
    }

    return receivedPacketSize;
}

int TcpConnection::getPacketSize()
{
    return receivedPacketSize;
}

byte TcpConnection::getPacket(byte *array)
{
    memcpy(array, receivedPacket, receivedPacketSize);
    return 0;
}