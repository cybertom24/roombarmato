/* LIBRERIE */
#include <TcpConnection.h>

/* COSTANTI */
#define PIN_LED 2
#define SSID "Roombarmato"
#define PASSWORD "e1m1-2077"
#define PORT 4000
#define SERIAL_BAUD_RATE 74880

/* FUNZIONI */
void turnOffLed();
void turnOnLed();

TcpConnection conn(SSID, PASSWORD, PORT);

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while(!Serial)
        ;   // Aspetta che la seriale venga inizializzata

    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    turnOnLed();

    conn.setup();
    
    turnOffLed();
}

void loop()
{
    // Aspetta un nuovo client aspettando indefinitamente
    while (!conn.waitClient(0))
        ;

    // Serial.println("Client connected");
    // Segnala che un client si è connesso mantenendo acceso il led
    turnOnLed();

    while (conn.clientConnected())
    {
        if (conn.checkPackets() > 0)
        {
            turnOffLed();
            int size = conn.getPacketSize();

            byte packetBuffer[size];
            conn.getPacket(packetBuffer);
            Serial.write(packetBuffer, size);
            turnOnLed();
        }

        if(Serial.available() > 0) {
            int size = Serial.available();
            byte message[size];
            Serial.readBytes(message, size);
            conn.send(message, size);
        }
    }

    // Serial.println("Client disconnected");
    // Spegni il led
    turnOffLed();
}

// Il led ha il controllo negato
void turnOffLed() {
    digitalWrite(PIN_LED, HIGH);
}

void turnOnLed() {
    digitalWrite(PIN_LED, LOW);
}