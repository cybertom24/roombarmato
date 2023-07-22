/* LIBRERIE */
#include "../../libraries/arduino-lib/Command/src/Command.h"
#include <TcpConnection.h>

/* COSTANTI */
#define PIN_LED 2
#define SSID "Roombarmato"
#define PASSWORD "e1m1-2077"
#define PORT 4000

TcpConnection conn(SSID, PASSWORD, PORT);

void setup()
{
    Serial.begin(74880);
    Serial.println("Booting up...");

    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    digitalWrite(PIN_LED, LOW);

    conn.setup();
    Serial.println("Connection created");
    digitalWrite(PIN_LED, HIGH);
}

void loop()
{
    if (Serial.available())
    {
        digitalWrite(PIN_LED, LOW);

        byte message[COMMAND_SIZE];
        Serial.readBytes(message, COMMAND_SIZE);
        // conn.send(message);

        digitalWrite(PIN_LED, HIGH);
    }

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
            byte *command = (byte *)malloc(sizeof(byte) * size);
            conn.getPacket(command);
            for (int i = 0; i < size; i++)
            {
                Serial.print((char) command[i]);
            }
            digitalWrite(PIN_LED, HIGH);
        }
    }
}