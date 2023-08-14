/* LIBRERIE */
#include <TcpConnection.h>
#include "../../libraries/arduino-lib/Command/src/Command.h"
#include <SecureSerialHW.h>
#include <string.h>

/* COSTANTI */
#define PIN_LED             2
#define SSID                "Roombarmato"
#define PASSWORD            "e1m1-2077"
#define PORT                4000
#define SERIAL_BAUD_RATE    38400
#define COMMAND_HOLDER_SIZE 64
#define MESSAGE_LENGTH      (COMMAND_SIZE)
#define TIMEOUT_SERIAL      10
#define ECC_SIZE            4

/* FUNZIONI */
void turnOffLed();
void turnOnLed();
void handleTCPpacket();
void handleSerialPacket();
void holdCommand(uint8_t *command);
void retrieveCommand(uint8_t *destination);
void resetHolder();
void sendCommandTCP(Command command);
void sendCommandSerial(Command command);

TcpConnection tcp(SSID, PASSWORD, PORT);

// Questo buffer (holder) conterrà tutti i comandi da spedire all'arduino quando sarà pronto.
uint8_t commandHolder[COMMAND_HOLDER_SIZE * COMMAND_SIZE];
// L'indice di scrittura scrive i nuovi comandi arrivati dall'esp, quello di lettura li manda all'arduino.
int commandHolderReadingIndex = 0, commandHolderWritingIndex = 0;
int commandsToBeRead = 0;
boolean arduinoReady = false;

unsigned long timeout = 0;
boolean timeoutActive = false;

SecureSerialHW::SSHW<MESSAGE_LENGTH, ECC_SIZE, TIMEOUT_SERIAL> sserial;

void setup()
{
    sserial.begin(&Serial, SERIAL_BAUD_RATE);
    while (!Serial)
        ; // Aspetta che la seriale venga inizializzata

    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    turnOnLed();

    tcp.setup();
    resetHolder();

    turnOffLed();
}

void loop()
{
    // Aspetta un nuovo client aspettando indefinitamente
    while (!tcp.waitClient(0))
        ;

    // Serial.println("Client connected");
    // Segnala che un client si è connesso mantenendo acceso il led
    turnOnLed();
    sendCommandSerial(Command::makeCommand(CODE_CONNECTED));

    while (tcp.clientConnected())
    {
        if (tcp.available() >= COMMAND_SIZE)
            handleTCPpacket();

        if (sserial.available() > 0)
            handleSerialPacket();

        // Se arduino è pronto a ricevere invia il prossimo comando disponibile
        /* if (arduinoReady && commandsToBeRead > 0)
        {
            uint8_t command[COMMAND_SIZE];
            retrieveCommand(command);
            Serial.write(command, COMMAND_SIZE);
            arduinoReady = false;
        } */

        sserial.handleCleaness();
    }

    sendCommandSerial(Command::makeCommand(CODE_DISCONNECTED));
    // resetHolder();

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

    uint8_t packet[COMMAND_SIZE];
    tcp.readBytes(packet, COMMAND_SIZE);
    // Se è della dimensione giusta copialo nel commandHolder, sennò scartalo. Indica che qualcosa di nuovo è arrivato
    /* if (commandsToBeRead == COMMAND_HOLDER_SIZE)
        sendCommandTCP(Command::makeCommand(CODE_HOLDER_FULL));
    else
        holdCommand(packet); */

    sserial.sendPacket(packet, COMMAND_SIZE);

    turnOnLed();
}

void handleSerialPacket()
{
    uint8_t message[MESSAGE_LENGTH];
    sserial.getPacket(message);
    Command c(message);

    if (c.isRight())
    {
        if (c.code() == CODE_SYNC)
            arduinoReady = true;
        else
            tcp.send(message, COMMAND_SIZE);
    }
}

void holdCommand(uint8_t *command)
{
    memcpy(&commandHolder[commandHolderWritingIndex], command, COMMAND_SIZE);
    commandHolderWritingIndex += COMMAND_SIZE;
    commandsToBeRead++;
    // Se supera il limite fallo tornare all'inizio
    if (commandHolderWritingIndex >= COMMAND_HOLDER_SIZE)
        commandHolderWritingIndex = 0;
}

// Copia il nuovo comando da inviare sulla seriale nell'indirizzo passato
void retrieveCommand(uint8_t *destination)
{
    memcpy(destination, &commandHolder[commandHolderReadingIndex], COMMAND_SIZE);
    commandsToBeRead--;
    commandHolderReadingIndex += COMMAND_SIZE;
    // Se supera il limite fallo tornare all'inizio
    if (commandHolderReadingIndex >= COMMAND_HOLDER_SIZE)
        commandHolderReadingIndex = 0;
}

void resetHolder()
{
    commandHolderReadingIndex = 0;
    commandHolderWritingIndex = 0;
    commandsToBeRead = 0;
    arduinoReady = false;
}

void sendCommandTCP(Command command)
{
    tcp.send(command.buffer, COMMAND_SIZE);
}

void sendCommandSerial(Command command)
{
    sserial.sendPacket(command.buffer, COMMAND_SIZE);
}