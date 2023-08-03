// TEST PER ESP01S

/* LIBRERIE */
#include <TcpConnection.h>
#include "../../../libraries/arduino-lib/Command/src/Command.h"
// #include "../../../libraries/arduino-lib/Arduino-FEC/src/RS-FEC.h"
#include <string.h>

/* COSTANTI */
#define PIN_LED             2
#define SSID                "Roombarmato"
#define PASSWORD            "e1m1-2077"
#define PORT                4000
#define SERIAL_BAUD_RATE    57600
#define COMMAND_HOLDER_SIZE 64
#define TIMEOUT_SERIAL      1000
#define ECC_LENGTH          4               // I byte possibili da sistemare sono massimo ECC / 2

/* FUNZIONI */
void turnOffLed();
void turnOnLed();
void handleTCPpacket();
void handleSerialPacket();
void sendCommandTCP(Command command);
void sendCommandSerial(Command command);
void handleSerialCleaness();
void clearSerial();

TcpConnection conn(SSID, PASSWORD, PORT);

// RS::ReedSolomon<COMMAND_SIZE, ECC_LENGTH> rs;
// I messaggi codificati con ECC saranno lunghi COMMAND_SIZE + ECC_LENTGH

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial)
        ; // Aspetta che la seriale venga inizializzata

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
    sendCommandSerial(Command::makeCommand(CODE_CONNECTED));

    while (conn.clientConnected())
    {
        if (conn.checkPackets() > 0)
            handleTCPpacket();
    }

    sendCommandSerial(Command::makeCommand(CODE_DISCONNECTED));

    // Serial.println("Client disconnected");
    // Spegni il led
    turnOffLed();
}

// Il led ha il controllo negato
void turnOffLed()
{
    digitalWrite(PIN_LED, HIGH);
}

void turnOnLed()
{
    digitalWrite(PIN_LED, LOW);
}

void handleTCPpacket()
{
    turnOffLed();

    int size = conn.getPacketSize();    
    uint8_t packet[size];
    conn.getPacket(packet);
    
    /* uint8_t encoded[COMMAND_SIZE + ECC_LENGTH];
    rs.Encode(packet, encoded);
    Serial.write(encoded, COMMAND_SIZE + ECC_LENGTH); */

    Serial.println(size, DEC);
    Serial.write(packet, size);

    turnOnLed();
}

void handleSerialPacket()
{
    uint8_t message[COMMAND_SIZE];
    Serial.readBytes(message, COMMAND_SIZE);
    Command c(message);
}

void sendCommandTCP(Command command)
{
    conn.send(command.buffer, COMMAND_SIZE);
}

void sendCommandSerial(Command command)
{
    Serial.write(command.buffer, COMMAND_SIZE);
}
