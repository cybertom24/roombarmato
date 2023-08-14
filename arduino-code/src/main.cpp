/**
 * @file main.cpp
 * @author cybertom
 * @brief
 * @version 0.1
 * @date 2023-07-22
 *
 * @copyright Copyright (c) 2023
 *
 * Note:
 *  - Servo:
 *      angolo di chiusura: 5
 *      angolo di apertura: 65
 */

/* LIBRERIE */
#include "../../libraries/arduino-lib/Command/src/Command.h"
#include <MP3Serial.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <string.h>
#include <SecureSerialSW.h>

/* COSTANTI */
#define DEBUG false
#define SUPER false
uint8_t expected[] = {0x00, 0xFF, 0xEA, 0xAE, 0x3C, 0xC3};

#define ON  HIGH
#define OFF LOW
// Definizione dei pin utilizzati e delle loro funzioni
// "MY" sta dal punto di vista di Arduino, ovvero "MY_TX_ESP" vuol dire che fa da pin trasmettitore per arduino e quindi ricevente per l'ESP
#define MY_RX_ESP 2
#define MY_TX_ESP 3
// Per l'MP3 si usa la seriale di default
// #define MY_RX_MP3 4
// #define MY_TX_MP3 5
// Motori
#define PIN_MOTORE_DX 11
#define PIN_MOTORE_SX 5
#define PIN_DX_DIR1   10
#define PIN_DX_DIR2   12
#define PIN_SX_DIR1   8
#define PIN_SX_DIR2   7
// Servo
#define PIN_SERVO_DX 6
// Razzi
#define PIN_RAZZO_0 A0
#define PIN_RAZZO_1 A1
#define PIN_RAZZO_2 A2
#define PIN_RAZZO_3 A3
const int PIN_RAZZI[] = {PIN_RAZZO_0, PIN_RAZZO_1, PIN_RAZZO_2, PIN_RAZZO_3};
// Altri pin
#define PIN_FINECORSA A4
#define PIN_LED_OCCHI A5
#define PIN_CHK_VBAT  A6
// Varie
#define ESP_BAUD_RATE           38400
#define MESSAGE_LENGTH          (COMMAND_SIZE)
#define TIMEOUT_SERIAL          10
#define ECC_SIZE                4
#define SERIAL_BAUD_RATE        9600
#define TIMEOUT_ESP_SERIAL      200
#define MIN_VBAT                43
#define MAX_VBAT                229
#define CLOSED_LID_ANGLE        5
#define OPENED_LID_ANGLE        65
#define BATTERY_LOW_DELAY_ERROR 2000
#define CANNA_ACCESA_DELAY      200
#define MOTOR_DELAY_AUTO_OFF    3000

/* FUNZIONI */
void    execute(Command command);
int     checkBattery();
void    openLid();
void    closeLid();
boolean isOpen();
boolean batteryLow();
void    sendCommand(Command command);
boolean shoot(uint8_t which);
void    setEyes(boolean status);
void    move(uint8_t potDx, uint8_t potSx);
void    stopMotor();

/* VARIABILI */
// Oggetti
SoftwareSerial espSerial(MY_RX_ESP, MY_TX_ESP);
MP3Serial      mp3;
SecureSerialSW::SSSW<MESSAGE_LENGTH, ECC_SIZE, TIMEOUT_SERIAL> esp;
Servo          servoDx;

// Timeout
unsigned long batteryLowTimeout = 0;
unsigned long motorTimeout = 0;
unsigned long readyCommandTimeout = 0;

boolean readyCommandSent = false;

struct Status
{
    uint8_t motorDx, motorSx;
    boolean eyesLit;
    boolean lidOpen;
    uint8_t rockets;
} status;

void setup()
{
    pinMode(PIN_MOTORE_DX, OUTPUT);
    pinMode(PIN_MOTORE_SX, OUTPUT);
    pinMode(PIN_DX_DIR1,   OUTPUT);
    pinMode(PIN_DX_DIR2,   OUTPUT);
    pinMode(PIN_SX_DIR1,   OUTPUT);
    pinMode(PIN_SX_DIR2,   OUTPUT);
    pinMode(LED_BUILTIN,   OUTPUT);
    pinMode(PIN_CHK_VBAT,  INPUT);
    pinMode(PIN_FINECORSA, INPUT);
    for (int pin : PIN_RAZZI)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    // Inizializzazione delle 2 seriali, quella per l'esp (esp) e quella per l'mp3 (mp3)
    esp.begin(&espSerial, ESP_BAUD_RATE);

    Serial.begin(ESP_BAUD_RATE);

    /* if (DEBUG)
        Serial.begin(ESP_BAUD_RATE);
    else
        mp3.begin();
 */
    // Indica ad arduino che la seriale su cui bisogna concentrarsi ad ascoltare è quella per l'esp
    // Quella per l'mp3 verrà ignorata in caso di dati in arrivo
    // (non è un problema visto che non arriveranno mai dati)
    espSerial.listen();

    servoDx.attach(PIN_SERVO_DX);
    closeLid();
    stopMotor();
    setEyes(OFF);

    if (DEBUG)
        Serial.println("> Setup finito");
}

void loop()
{
    // Controlla che il valore della batteria sia accettabile
    if (batteryLow())
    {
        // La batteria è troppo bassa
        // Invia ogni tot secondi il messaggio che la batteria è troppo bassa
        if (batteryLowTimeout + BATTERY_LOW_DELAY_ERROR < millis())
        {
            sendCommand(Command::makeCommand(CODE_BATTERY_LOW));
            batteryLowTimeout = millis();
        }
        // Interrompi qualsiasi tipo di azione e spegni gli occhi
        stopMotor();
        setEyes(OFF);
        return;
    }

    // Invia il messaggio SYNC quando si è pronti o è passato troppo tempo da una risposta
    /* if (!readyCommandSent || millis() > readyCommandTimeout + TIMEOUT_ESP_SERIAL)
    {
        //if (DEBUG) Serial.println("> Invio comando di SYNC");

        sendCommand(Command::makeCommand(CODE_SYNC));
        readyCommandSent = true;
        readyCommandTimeout = millis();
    } */

    if (esp.available() > 0)
    {
        readyCommandSent = false;

        uint8_t packet[COMMAND_SIZE];

        if (esp.getPacket(packet))
        {
            // Non ci sono errori nel pacchetto
            
            boolean right = true;
            for(int i = 0; i < COMMAND_SIZE; i++) {
                if(packet[i] != expected[i]) {
                    right = false;
                    break;
                }
            }

            if(right)
                Serial.println("0");
            else
                Serial.println("1");
            
            Command c(packet);

            if (DEBUG)
            {
                if (SUPER)
                {
                    Serial.print("> Arrivato:");
                    for (int i = 0; i < COMMAND_SIZE; i++)
                    {
                        Serial.print(" ");
                        Serial.print(packet[i], HEX);
                    }
                    Serial.println();
                }
            }

            if (c.isRight())
                execute(c);
        }
        else
        {
            // Il pacchetto contiene errori. Non può essere assolutamente considerato
            Serial.println("1");
        }
    }

    // Se non sono arrivati messaggi per il controllo dei motori entro tot secondi fermali
    if (status.motorDx > 1 && status.motorSx > 1 && millis() >= motorTimeout + MOTOR_DELAY_AUTO_OFF)
        stopMotor();

    esp.handleCleaness();
}

void execute(Command command)
{

    if (DEBUG && SUPER)
        Serial.println("> Eseguendo comando");

    uint8_t data[command.size()];
    command.data(data);

    switch (command.code())
    {
    case CODE_PLAY:
    {
        mp3.play();

        if (DEBUG)
            Serial.println("> play musica");
        break;
    }
    case CODE_PLAY_INDEX:
    { // Da rimuovere? Non ricordo perchè. Investigare!
        mp3.playIndex((int)data[0] + 1);

        if (DEBUG)
            Serial.println("> play index");
        break;
    }
    case CODE_PAUSE:
    {
        mp3.pause();

        if (DEBUG)
            Serial.println("> pausa musica");
        break;
    }
    case CODE_NEXT_SONG:
    {
        mp3.next();

        if (DEBUG)
            Serial.println("> prossima canzone");
        break;
    }
    case CODE_PREVIOUS_SONG:
    {
        mp3.previous();

        if (DEBUG)
            Serial.println("> canzone precedente");
        break;
    }
    case CODE_SET_VOLUME:
    {
        mp3.volumeSet(data[0]);

        if (DEBUG)
            Serial.println("> imposta volume");
        break;
    }
    case CODE_MOVE:
    { // In data[0] è contenuta la potenza da dare al motore DX
        // In data[1] quella per il motore SX
        move(data[0], data[1]);
        break;
    }
    case CODE_OPEN_LID:
    {
        openLid();
        break;
    }
    case CODE_CLOSE_LID:
    {
        closeLid();
        break;
    }
    case CODE_FIRE:
    {
        boolean result = shoot(data[0]);
        // Se lo sparo non è andato a buon fine
        if (!result)
        {
            sendCommand(Command::makeCommand(CODE_LAUNCH_FAILURE));
        }
    }
    break;
    case CODE_EYES:
    {
        setEyes(data[0]);
        break;
    }
    case CODE_CONNECTED:
    {
        if (DEBUG)
            Serial.println("> Qualcuno si è connesso");
        break;
    }
    case CODE_DISCONNECTED:
    {
        if (DEBUG)
            Serial.println("> Qualcuno si è disconnesso");
        break;
    }
    }
}

// Ritorna la percentuale di carica della batteria in un numero da 0 a 100
int checkBattery()
{
    return map(analogRead(PIN_CHK_VBAT), MIN_VBAT, MAX_VBAT, 0, 100);
}

void openLid()
{
    servoDx.write(OPENED_LID_ANGLE);
    status.lidOpen = true;

    if (DEBUG)
        Serial.println("> Aprendo il coperchio");
}

void closeLid()
{
    servoDx.write(CLOSED_LID_ANGLE);
    status.lidOpen = false;

    if (DEBUG)
        Serial.println("> Chiudendo il coperchio");
}

boolean isOpen()
{
    return digitalRead(PIN_FINECORSA);
}

boolean batteryLow()
{
    // return analogRead(PIN_CHK_VBAT) <= MIN_VBAT;
    return false;
}

void sendCommand(Command command)
{
    esp.sendPacket(command.buffer, COMMAND_SIZE);
}

void move(uint8_t potDx, uint8_t potSx)
{
    // Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
    // mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
    // Recupera la direzione in cui mandare i motori
    boolean dirDx = potDx & 0x01,
            dirSx = potSx & 0x01;
    // Imposta la direzione del motore DX
    digitalWrite(PIN_DX_DIR1, dirDx);
    digitalWrite(PIN_DX_DIR2, !dirDx);
    // Imposta la direzione del motore SX
    digitalWrite(PIN_SX_DIR1, dirSx);
    digitalWrite(PIN_SX_DIR2, !dirSx);
    // Imposta la potenza data ai motori (0xFE = 11111110B -> rende sempre zero il LSB)
    analogWrite(PIN_MOTORE_DX, potDx & 0xFE);
    analogWrite(PIN_MOTORE_SX, potSx & 0xFE);

    status.motorDx = potDx;
    status.motorSx = potSx;
    motorTimeout = millis();

    if (DEBUG)
        Serial.println(potDx);
}

void stopMotor()
{
    // Quando si fermano i motori, la direzione è preimpostata in avanti
    move(1, 1);
}

// Spara con la canna selezionata dall'indice passato
// Ritorna se lo sparo è andato a buon fine oppure no
boolean shoot(uint8_t which)
{
    // Se non è aperto non sparare
    if (!isOpen())
        return false;

    digitalWrite(PIN_RAZZI[which], HIGH);
    if (DEBUG)
    {
        Serial.print("> Accendendo canna ");
        Serial.println(which, DEC);
    }

    delay(CANNA_ACCESA_DELAY);
    digitalWrite(PIN_RAZZI[which], LOW);
    return true;

    if (DEBUG)
        Serial.println("> Spegnendo canna");
}

void setEyes(boolean lit)
{
    digitalWrite(PIN_LED_OCCHI, lit);
    status.eyesLit = lit;

    if (DEBUG)
    {
        Serial.print("> Impostando occhi a ");
        Serial.println(lit, BIN);
    }
}