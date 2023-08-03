#include "TcpConnection.h"
#include <ESP8266WiFi.h>

TcpConnection::TcpConnection(String mySsid, String myPassword, int myPort)
{
    MYSSID = mySsid;
    MYPASSWORD = myPassword;
    MYPORT = myPort;
}

void TcpConnection::setup()
{
    if (!Serial)
    {
        Serial.begin(SERIAL_BAUD_RATE);
        while (!Serial)
            ; // Wait
    }
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
    tcpServer = new WiFiServer((uint16_t)MYPORT);
    tcpServer->begin();

    if (debug)
    {
        Serial.printf("TCP socket open!\n");
        Serial.printf("\t> IP: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("\t> Port: %d\n", MYPORT);
    }
}

boolean TcpConnection::waitClient(unsigned long timeout)
{
    // If timeout is 0 wait indefinetly
    if (timeout == 0)
    {
        do
        {
            client = tcpServer->accept();
        } while (!client);
    }
    else
    {
        // Else set a maximum waiting time
        unsigned long startMicroseconds = micros();
        do
        {
            client = tcpServer->accept();
        } while (!client && startMicroseconds + timeout < micros());
    }

    if (client)
        return true;
    else
        return false;
}

boolean TcpConnection::clientConnected()
{
    if (!client)
        return false;

    if(!client.connected()) 
    {
        client.stop();
        return false;
    }

    return true;
}

int TcpConnection::checkPackets()
{
    if (!clientConnected())
        return -1;

    if (client.available() == 0)
        return -1;

    receivedPacketSize = client.available();

    // Truncate the packet received
    if (receivedPacketSize > MAX_PACKET_SIZE)
        receivedPacketSize = MAX_PACKET_SIZE;

    client.read(bufferIn, receivedPacketSize);

    if (debug)
    {
        Serial.printf("Received packet from %s:%d of lenght: %d\n", client.remoteIP().toString().c_str(), client.remotePort(), receivedPacketSize);
    }

    return receivedPacketSize;
}

int TcpConnection::getPacketSize()
{
    return receivedPacketSize;
}

uint8_t TcpConnection::getPacket(uint8_t *array)
{
    memcpy(array, bufferIn, receivedPacketSize);
    return 0;
}

boolean TcpConnection::send(uint8_t *message, int length) 
{
    if(!clientConnected())
        return false;

    int byteWritten = client.write(message, length);
    return byteWritten == length;
}