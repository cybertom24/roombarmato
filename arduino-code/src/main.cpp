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

// Definisci DEBUG per attivare l'opzione
// #define DEBUG

/* LIBRERIE */
#include "../../libraries/arduino-lib/Command/src/Command.h"
#include <MP3Serial.h>
#include <Servo.h>
#include <string.h>
#include <SecureSerialHW.h>

/* COSTANTI */

// Definizione dei pin utilizzati e delle loro funzioni
// "MY" sta dal punto di vista di Arduino, ovvero "MY_TX_MP3" vuol dire che fa da pin trasmettitore per arduino e quindi ricevente per l'MP3
// Per l'MP3 si usa software serial
#define MY_RX_MP3 2
#define MY_TX_MP3 3

// Motori
#define PIN_MOTORE_DX 11
#define PIN_MOTORE_SX 5
#define PIN_DX_DIR1   10
#define PIN_DX_DIR2   12
#define PIN_SX_DIR1   8
#define PIN_SX_DIR2   7
// Servo
#define PIN_SERVO     6
// Razzi
#define NUM_RAZZI   4
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
#define ESP_BAUD_RATE           115200
#define MESSAGE_LENGTH          (COMMAND_SIZE)
#define TIMEOUT_SERIAL          10
#define ECC_SIZE                4
#define TIMEOUT_ESP_SERIAL      20
#define MIN_VBAT                790
#define MAX_VBAT                980
#define CLOSED_LID_ANGLE        5
#define OPENED_LID_ANGLE        65
#define BATTERY_LOW_DELAY_ERROR 2000
#define CANNA_ACCESA_DELAY      2000
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
void    blocca();
void    updateServo();

/* VARIABILI */
// Oggetti
MP3Serial      mp3(MY_RX_MP3, MY_TX_MP3);
SecureSerialHW::SSHW<MESSAGE_LENGTH, ECC_SIZE, TIMEOUT_SERIAL> esp;
Servo          servo;

// Holds the status of all the elements attached
struct Status
{
    uint8_t motorDx = 1, motorSx = 1;
    boolean eyesLit = LOW;
    boolean lidOpen = false;
    boolean connected = false;
    boolean musicPlaying = false;
    boolean rockets[NUM_RAZZI] = { false };
} status;

// For various timeouts inside the program
struct Timeout
{
    unsigned long batteryLow   = 0;
    unsigned long motor        = 0;
    unsigned long readyCommand = 5000;
    unsigned long rockets[4]   = { 0 };
} timeout;

boolean readyCommandSent = false;

// Metrics
unsigned long lastMicros = 0;
struct LoopTime {
    unsigned long max = 0;
    unsigned long min = INT32_MAX;
    unsigned long tot = 0;
    unsigned long takes = 0;
} loopTime;

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
    esp.begin(&Serial, ESP_BAUD_RATE);
    mp3.begin();

    servo.attach(PIN_SERVO);
    
    blocca();

    #ifdef DEBUG
        Serial.println("> Setup finito");
    #endif
}

void loop()
{
    lastMicros = micros();

    // Controlla che il valore della batteria sia accettabile
    if (batteryLow())
    {
        // La batteria è troppo bassa
        // Invia ogni tot secondi il messaggio che la batteria è troppo bassa
        if (timeout.batteryLow + BATTERY_LOW_DELAY_ERROR < millis())
        {
            sendCommand(Command::makeCommand(CODE_BATTERY_LOW));
            timeout.batteryLow = millis();
            
            // Interrompi qualsiasi tipo di azione e spegni gli occhi
            blocca();
        }
        return;
    }

    // Invia il messaggio SYNC quando si è pronti o è passato troppo tempo da una risposta
    /* if (!readyCommandSent || millis() > readyCommandTimeout + TIMEOUT_ESP_SERIAL)
    {
        //if (DEBUG) Serial.println("> Invio comando di SYNC");

        Serial.print("max: ");
        Serial.println(loopTime.max, DEC);
        Serial.print("min: ");
        Serial.println(loopTime.min, DEC);
        Serial.print("mid: ");
        Serial.println(loopTime.tot / loopTime.takes, DEC);

        sendCommand(Command::makeCommand(CODE_CONNECTION_OK));
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
            Command c(packet);

            #ifdef DEBUG
                Serial.print("> Arrivato");
                for (int i = 0; i < COMMAND_SIZE; i++)
                {
                    Serial.print(" ");
                    Serial.print(packet[i], HEX);
                }
                Serial.println();
            #endif

            if (c.isRight())
                execute(c);
            else
                sendCommand(Command::makeCommand(CODE_WRONG_PACKET));
        }
        else
        {
            // Il pacchetto contiene errori. Non può essere assolutamente considerato
            sendCommand(Command::makeCommand(CODE_WRONG_PACKET));
        }
    }

    // Se non sono arrivati messaggi per il controllo dei motori entro tot secondi fermali
    if ((status.motorDx > 1 || status.motorSx > 1) && millis() >= timeout.motor + MOTOR_DELAY_AUTO_OFF)
        stopMotor();

    // Spegni le canne se è passato abbastanza tempo
    // Controlla tutte le canne
    for(int i = 0; i < NUM_RAZZI; i++)
    {
        if(status.rockets[i] && millis() > timeout.rockets[i] + CANNA_ACCESA_DELAY)
        {
            digitalWrite(PIN_RAZZI[i], LOW);
            status.rockets[i] = false;
            
            #ifdef DEBUG
                Serial.print("> Spegnendo canna ");
                Serial.println(i, DEC);
                Serial.print("> Stato rockets: ");
                for(int j = 0; j < NUM_RAZZI; j++)
                    Serial.println(status.rockets[j], BIN);
            #endif
        }
    }

    esp.handleCleaness();

    // Controllo della durata
    unsigned long dur = micros() - lastMicros;
    if (loopTime.max < dur)
        loopTime.max = dur;
    if (loopTime.min > dur)
        loopTime.min = dur;
    loopTime.takes++;
    loopTime.tot += dur;
}

void execute(Command command)
{

    #ifdef DEBUG
        Serial.println("> Eseguendo comando");
    #endif

    uint8_t data[command.size()];
    command.data(data);

    switch (command.code())
    {
    case CODE_PLAY:
    {
        mp3.play();
        status.musicPlaying = true;
        updateServo();

        #ifdef DEBUG
            Serial.println("> play musica");
        #endif
        
        break;
    }
    case CODE_PLAY_INDEX:
    {
        mp3.playIndex((int)data[0] + 1);
        updateServo();

        #ifdef DEBUG
            Serial.println("> play index");
        #endif

        break;
    }
    case CODE_PAUSE:
    {
        mp3.pause();
        status.musicPlaying = false;
        updateServo();

        #ifdef DEBUG
            Serial.println("> pausa musica");
        #endif

        break;
    }
    case CODE_NEXT_SONG:
    {
        mp3.next();
        updateServo();

        #ifdef DEBUG
            Serial.println("> prossima canzone");
        #endif

        break;
    }
    case CODE_PREVIOUS_SONG:
    {
        mp3.previous();
        updateServo();

        #ifdef DEBUG
            Serial.println("> canzone precedente");
        #endif

        break;
    }
    case CODE_SET_VOLUME:
    {
        mp3.volumeSet(data[0]);
        updateServo();

        #ifdef DEBUG
            Serial.println("> imposta volume");
        #endif

        break;
    }
    case CODE_MOVE:
    { 
        // In data[0] è contenuta la potenza da dare al motore DX
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
        break;
    }
    case CODE_EYES:
    {
        setEyes(data[0]);
        break;
    }
    case CODE_CONNECTED:
    {
        status.connected = true;

        #ifdef DEBUG
            Serial.println("> Qualcuno si è connesso");
        #endif

        break;
    }
    case CODE_DISCONNECTED:
    {
        status.connected = false;
        blocca();

        #ifdef DEBUG
            Serial.println("> Qualcuno si è disconnesso");
        #endif
        
        break;
    }
    case CODE_CHECK_BATTERY:
    {
        sendCommand(Command::makeCommand(CODE_BATTERY_STATUS, checkBattery()));
        break;
    }
    case CODE_CHECK_LID:
    {
        uint8_t lidStatus = 0;
        if(isOpen())
            lidStatus = 1;
        
        sendCommand(Command::makeCommand(CODE_LID_STATUS, lidStatus));
        break;
    }
    case CODE_CHECK_CONNECTION:
    {
        sendCommand(Command::makeCommand(CODE_CONNECTION_OK));
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
    servo.write(OPENED_LID_ANGLE);
    status.lidOpen = true;

    #ifdef DEBUG
        Serial.println("> Aprendo il coperchio");
    #endif
}

void closeLid()
{
    servo.write(CLOSED_LID_ANGLE);
    status.lidOpen = false;

    #ifdef DEBUG
        Serial.println("> Chiudendo il coperchio");
    #endif
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
    digitalWrite(PIN_DX_DIR1, !dirDx);
    digitalWrite(PIN_DX_DIR2, dirDx);
    // Imposta la direzione del motore SX
    digitalWrite(PIN_SX_DIR1, dirSx);
    digitalWrite(PIN_SX_DIR2, !dirSx);
    // Imposta la potenza data ai motori (0xFE = 11111110B -> rende sempre zero il LSB)
    analogWrite(PIN_MOTORE_DX, potDx & 0xFE);
    analogWrite(PIN_MOTORE_SX, potSx & 0xFE);

    status.motorDx = potDx;
    status.motorSx = potSx;
    timeout.motor  = millis();

    #ifdef DEBUG
        Serial.print("> motori: ");
        Serial.print(potDx, HEX);
        Serial.print(" - ");
        Serial.println(potSx, HEX);
    #endif
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

    // Accendi solo una canna alla volta
    for(int i = 0; i < NUM_RAZZI; i++)
        if(status.rockets[i])
            return false;

    // Accendi la canna
    digitalWrite(PIN_RAZZI[which], HIGH);
    
    // Segna che la canna è stata accesa e indica quando è stata accesa
    status.rockets[which]  = true;
    timeout.rockets[which] = millis();

    #ifdef DEBUG
        Serial.print("> Accendendo canna ");
        Serial.println(which, DEC);
        Serial.print("> Stato rockets: ");
        for(int i = 0; i < NUM_RAZZI; i++)
            Serial.println(status.rockets[i], BIN);
    #endif

    return true;
}

void setEyes(boolean lit)
{
    digitalWrite(PIN_LED_OCCHI, lit);
    status.eyesLit = lit;

    #ifdef DEBUG
        Serial.print("> Impostando occhi a ");
        Serial.println(lit, BIN);
    #endif
}

void blocca()
{
    stopMotor();
    setEyes(LOW);
    mp3.pause();
    status.musicPlaying = false;
    closeLid();
}

void updateServo()
{
    if(status.lidOpen)
        openLid();
    else
        closeLid();
}