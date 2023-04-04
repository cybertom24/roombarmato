/* LIBRERIE */
#include "UdpConnection.h"
#include "Command.h"

/* COSTANTI */
#define PIN_LED 2
#define SSID "Roombarmato"
#define PASSWORD "e1m1-2077"
#define PORT 4000

UdpConnection conn(SSID, PASSWORD, PORT);

void setup()
{
    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    digitalWrite(PIN_LED, LOW);

    conn.setup();

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
    // Controlla se ci sono pacchetti arrivati
    if (conn.checkPackets() > 0)
    {
        digitalWrite(PIN_LED, LOW);

        byte *command = (byte *)malloc(sizeof(byte) * conn.getPacketSize());
        conn.getPacket(command);
        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            Serial.write(command[i]);
        }
        free(command);

        digitalWrite(PIN_LED, HIGH);
    }
}
