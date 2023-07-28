/* LIBRERIE */
#include <TcpConnection.h>
#include "../../libraries/arduino-lib/Command/src/Command.h"

/* COSTANTI */
#define PIN_LED             2
#define SSID                "Roombarmato"
#define PASSWORD            "e1m1-2077"
#define PORT                4000
#define SERIAL_BAUD_RATE    76800
#define COMMAND_HOLDER_SIZE 64
#define TIMEOUT_SERIAL      1000

/* FUNZIONI */
void turnOffLed();
void turnOnLed();
void handleTCPpacket();
void handleSerialPacket();
void holdCommand(byte *command);
void retrieveCommand(byte *destination);
void resetHolder();
void sendCommandTCP(Command command);
void sendCommandSerial(Command command);
void handleSerialCleaness();
void clearSerial();

TcpConnection conn(SSID, PASSWORD, PORT);

// Questo buffer (holder) conterrà tutti i comandi da spedire all'arduino quando sarà pronto.
uint8_t commandHolder[COMMAND_HOLDER_SIZE * COMMAND_SIZE];
// L'indice di scrittura scrive i nuovi comandi arrivati dall'esp, quello di lettura li manda all'arduino.
int commandHolderReadingIndex = 0, commandHolderWritingIndex = 0;
int commandsToBeRead = 0;
boolean arduinoReady = false;

unsigned long timeout = 0;
boolean timeoutActive = false;

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial)
        ; // Aspetta che la seriale venga inizializzata

    pinMode(PIN_LED, OUTPUT);
    // Durante il boot il led è acceso, si spegne a boot completato
    turnOnLed();

    conn.setup();
    resetHolder();

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

        if (Serial.available() >= COMMAND_SIZE)
            handleSerialPacket();

        // Se arduino è pronto a ricevere invia il prossimo comando disponibile
        if (arduinoReady && commandsToBeRead > 0)
        {
            byte command[COMMAND_SIZE];
            retrieveCommand(command);
            Serial.write(command, COMMAND_SIZE);
            arduinoReady = false;
        }

        handleSerialCleaness();
    }

    clearSerial();
    sendCommandSerial(Command::makeCommand(CODE_DISCONNECTED));
    resetHolder();

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
    byte packet[size];
    conn.getPacket(packet);
    // Se è della dimensione giusta copialo nel commandHolder, sennò scartalo. Indica che qualcosa di nuovo è arrivato
    if (size == COMMAND_SIZE)
    {
        // Se ho riempito l'holder dei comandi invia un messaggio di errore
        if (commandsToBeRead == COMMAND_HOLDER_SIZE)
            sendCommandTCP(Command::makeCommand(CODE_HOLDER_FULL));
        else
            holdCommand(packet);
    }

    turnOnLed();
}

void handleSerialPacket()
{
    uint8_t message[COMMAND_SIZE];
    Serial.readBytes(message, COMMAND_SIZE);
    Command c(message);

    if (c.isRight())
    {
        if (c.code() == CODE_SYNC)
            arduinoReady = true;
        else
            conn.send(message, COMMAND_SIZE);
    }

    timeoutActive = false;
}

void holdCommand(byte *command)
{
    memcpy(&commandHolder[commandHolderWritingIndex], command, COMMAND_SIZE);
    commandHolderWritingIndex += COMMAND_SIZE;
    commandsToBeRead++;
    // Se supera il limite fallo tornare all'inizio
    if (commandHolderWritingIndex >= COMMAND_HOLDER_SIZE)
        commandHolderWritingIndex = 0;
}

// Copia il nuovo comando da inviare sulla seriale nell'indirizzo passato
void retrieveCommand(byte *destination)
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
    conn.send(command.buffer, COMMAND_SIZE);
}

void sendCommandSerial(Command command)
{
    Serial.write(command.buffer, COMMAND_SIZE);
}

void handleSerialCleaness()
{
    if (Serial.available() % COMMAND_SIZE != 0)
    {
        // Se c'è qualcosa nel buffer ma non è del numero giusto per essere letto
        if (!timeoutActive)
        {
            // Fai partire il espTimeout se non è ancora partito
            timeout = millis();
            timeoutActive = true;
        }
        // Se è scaduto il timeout ripulisci la seriale
        else if (millis() >= timeout + TIMEOUT_SERIAL)
        {
            clearSerial();
            // E blocca il timeout
            timeoutActive = false;
        }
    }
}

void clearSerial()
{
    if (Serial.available() > 0)
    {
        uint8_t garbage[Serial.available()];
        Serial.readBytes(garbage, Serial.available());
    }
}