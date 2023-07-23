/* LIBRERIE */
#include "../../libraries/arduino-lib/Command/src/Command.h"
#include <TcpConnection.h>

/* COSTANTI */
#define PIN_LED 2
#define SSID "Roombarmato"
#define PASSWORD "e1m1-2077"
#define PORT 4000
#define SERIAL_BAUD_RATE 74880

TcpConnection conn(SSID, PASSWORD, PORT);

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while(!Serial)
        ;   // Wait

    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    digitalWrite(PIN_LED, LOW);

    conn.setup();
    
    digitalWrite(PIN_LED, HIGH);
}

void loop()
{
    // Aspetta un nuovo client
    while (!conn.waitClient(0))
        ;

    Serial.println("Client connected");

    while (conn.clientConnected())
    {
        if (conn.checkPackets() > 0)
        {
            digitalWrite(PIN_LED, LOW);

            int size = conn.getPacketSize();

            byte packetBuffer[size];
            conn.getPacket(packetBuffer);
            for (int i = 0; i < size; i++)
            {
                Serial.print((char) packetBuffer[i]);
            }
            digitalWrite(PIN_LED, HIGH);
        }

        if(Serial.available() > 0) {
            int size = Serial.available();
            byte message[size];
            Serial.readBytes(message, size);
            conn.send(message, size);
        }
    }

    Serial.println("Client disconnected");
}