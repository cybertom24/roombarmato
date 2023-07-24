/**
 * @file main.cpp
 * @author your name (you@domain.com)
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

/* COSTANTI */
// Definizione dei pin utilizzati e delle loro funzioni
// "MY" sta dal punto di vista di Arduino, ovvero "MY_TX_ESP" vuol dire che fa da pin trasmettitore per arduino e quindi ricevente per l'ESP
#define MY_RX_ESP 2
#define MY_TX_ESP 3
// Per l'MP3 si usa la seriale solita
// #define MY_RX_MP3 4
// #define MY_TX_MP3 5
// Motori
#define PIN_MOTORE_DX 10
#define PIN_MOTORE_SX 9
#define PIN_DX_DIR1 11
#define PIN_DX_DIR2 12
#define PIN_SX_DIR1 8
#define PIN_SX_DIR2 7
// Servo
#define PIN_SERVO_DX 6
#define PIN_SERVO_SX 5
// Razzi
#define PIN_RAZZO_0 A0
#define PIN_RAZZO_1 A1
#define PIN_RAZZO_2 A2
#define PIN_RAZZO_3 A3
// Altri pin
#define PIN_FINECORSA A4
#define PIN_LED_OCCHI A5
#define PIN_CHK_VBAT A6
// Varie
#define ESP_BAUD_RATE 74880
#define SERIAL_BAUD_RATE 9600
#define TIMEOUT_ESP_SERIAL 1000
#define MIN_VBAT 43
#define MAX_VBAT 229
#define CLOSED_LID_ANGLE 5
#define OPENED_LID_ANGLE 65

/* FUNZIONI */
void execute(Command command);
int checkBattery();
void openLid();
void closeLid();
boolean isOpen();

// Creazione dei due oggetti per il controllo delle due schede
SoftwareSerial esp(MY_RX_ESP, MY_TX_ESP);
MP3Serial mp3;

Servo servoDx;

unsigned long timeout = 0;
boolean timeoutActive = false;

void setup()
{
    pinMode(PIN_MOTORE_DX, OUTPUT);
    pinMode(PIN_MOTORE_SX, OUTPUT);
    pinMode(PIN_DX_DIR1, OUTPUT);
    pinMode(PIN_DX_DIR2, OUTPUT);
    pinMode(PIN_SX_DIR1, OUTPUT);
    pinMode(PIN_SX_DIR2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_CHK_VBAT, INPUT);
    pinMode(PIN_FINECORSA, INPUT);

    // Inizializzazione delle 2 seriali, quella per l'esp (esp) e quella per l'mp3 (mp3)
    esp.begin(ESP_BAUD_RATE);
    mp3.begin();

    // Indica ad arduino che la seriale su cui bisogna concentrarsi ad ascoltare è quella per l'esp
    // Quella per l'mp3 verrà ignorata in caso di dati in arrivo
    // (non è un problema visto che non arriveranno mai dati)
    esp.listen();

    servoDx.attach(PIN_SERVO_DX);
}

void loop_debug()
{
    if (esp.available() > 0)
    {
        int size = esp.available();
        byte bufferIn[size];
        esp.readBytes(bufferIn, size);

        Serial.print("> ");
        for (int i = 0; i < size; i++)
        {
            Serial.print((char)bufferIn[i]);
            // Serial.print(" ");
        }
        Serial.println("");
    }

    if (Serial.available() > 0)
    {
        int size = Serial.available();
        byte bufferOut[size];
        Serial.readBytes(bufferOut, size);

        esp.write(bufferOut, size);
    }
}

void loop()
{
    if (esp.available() >= COMMAND_SIZE)
    {
        timeoutActive = false;

        byte bufferIn[COMMAND_SIZE];
        esp.readBytes(bufferIn, COMMAND_SIZE);
        Command c(bufferIn);
        if (c.isRight())
            execute(c);
    }

    // Blocco di codice che ripulisce la seriale con l'esp in caso ci siano dei dati che non sono compatibili con i comandi
    if (esp.available() % COMMAND_SIZE != 0)
    {
        // Se c'è qualcosa nel buffer ma non è del numero giusto per essere letto
        if (!timeoutActive)
        {
            // Fai partire il timeout se non è ancora partito
            timeout = millis();
            timeoutActive = true;
        }
        else if (millis() >= timeout + TIMEOUT_ESP_SERIAL)
        {
            // Se è scaduto il timeout ripulisci la seriale
            byte garbage[esp.available()];
            esp.readBytes(garbage, esp.available());
            // E blocca il timeout
            timeoutActive = false;
        }
    }
}

void execute(Command command)
{
    byte data[command.size()];
    command.data(data);

    switch (command.code())
    {
    case CODE_PLAY:
        mp3.play();
        break;
    //        case Command::PLAY_INDEX:
    //          // !! DA RIMUOVERE !!
    //          mp3.Play_index((int) c._data[0] + 1);
    //          break;
    case CODE_PAUSE:
        mp3.pause();
        break;
    case CODE_NEXT_SONG:
        mp3.next();
        break;
    case CODE_PREVIOUS_SONG:
        mp3.previous();
        break;
    case CODE_SET_VOLUME:
        mp3.volumeSet(data[0]);
        break;
    case CODE_MOVE:
        /* In data[0] è contenuta la potenza da dare al motore DX
           In data[1] quella per il motore SX
           Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
           mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
        */
        // Recupera la direzione in cui mandare i motori
        bool dirX = data[0] & 0x01,
             dirY = data[1] & 0x01;
        // Imposta la direzione del motore DX
        digitalWrite(PIN_DX_DIR1, dirX);
        digitalWrite(PIN_DX_DIR2, !dirX);
        // Imposta la direzione del motore SX
        digitalWrite(PIN_SX_DIR1, dirY);
        digitalWrite(PIN_SX_DIR2, !dirY);
        // Imposta la potenza data ai motori (0xFE = 11111110B -> rende sempre zero il LSB)
        analogWrite(PIN_MOTORE_DX, data[0] & 0xFE);
        analogWrite(PIN_MOTORE_SX, data[1] & 0xFE);
        break;
    }
}

// Ritorna la percentuale di carica della batteria in un numero da 0 a 100
int checkBattery() {
    return map(analogRead(PIN_CHK_VBAT), MIN_VBAT, MAX_VBAT, 0, 100);
}

void openLid() {
    servoDx.write(OPENED_LID_ANGLE);
}

void closeLid() {
    servoDx.write(CLOSED_LID_ANGLE);
}

boolean isOpen() {
    return digitalRead(PIN_FINECORSA);
}