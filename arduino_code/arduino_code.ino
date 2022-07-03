#include "Command.h"
#include "MP3Serial.h"
#include <SoftwareSerial.h>

// Definizione dei pin utilizzati e delle loro funzioni
// "MY" sta dal punto di vista di Arduino, ovvero "MY_TX_ESP" vuol dire che fa da pin trasmettitore per arduino e quindi ricevente per l'ESP
#define MY_RX_ESP 2
#define MY_TX_ESP 3
#define MY_RX_MP3 4
#define MY_TX_MP3 5
#define PIN_MOTORE_DX 10
#define PIN_MOTORE_SX 9
#define PIN_DX_DIR1 11
#define PIN_DX_DIR2 12
#define PIN_SX_DIR1 8
#define PIN_SX_DIR2 7

#define ESP_BAUD_RATE 74880
#define SERIAL_BAUD_RATE 9600

// Creazione dei due oggetti per il controllo delle due schede
SoftwareSerial esp(MY_RX_ESP, MY_TX_ESP);
MP3Serial mp3(MY_RX_MP3, MY_TX_MP3);

void setup() {
  pinMode(PIN_MOTORE_DX, OUTPUT);
  pinMode(PIN_MOTORE_SX, OUTPUT);
  pinMode(PIN_DX_DIR1, OUTPUT);
  pinMode(PIN_DX_DIR2, OUTPUT);
  pinMode(PIN_SX_DIR1, OUTPUT);
  pinMode(PIN_SX_DIR2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Inizializzazione delle 3 seriali, quella per il debug (Serial), quella per l'esp (esp) e quella per l'mp3 player (mp3)
  esp.begin(ESP_BAUD_RATE);
  Serial.begin(SERIAL_BAUD_RATE);
  mp3.Begin();

  // Indica ad arduino che la seriale su cui bisogna concentrarsi ad ascoltare è quella per l'esp
  // Quella per l'mp3 verrà ignorate in caso di dati in arrivo
  // (non è un problema visto che non arriveranno mai dati)
  esp.listen();
}

void loop() {
  if (esp.available() > 0) {
    byte bufferIn[Command::SIZE];
    esp.readBytes(bufferIn, Command::SIZE);
    // Probabilmente troppo lento e causa il blocco del sistema
    // ** DA TESTARE **
//    for (int i = 0; i < Command::SIZE; i++) {
//      Serial.println(bufferIn[i], HEX);
//    }
    Command c(bufferIn);
    if (c.isRight) {
      switch (c._command) {
        case Command::PLAY:
          mp3.Play();
          break;
//        case Command::PLAY_INDEX:
//          // !! DA RIMUOVERE !!
//          mp3.Play_index((int) c._data[0] + 1);
//          break;
        case Command::PAUSE:
          mp3.Pause();
          break;
        case Command::NEXT_SONG:
          mp3.Next();
          break;
        case Command::PREVIOUS_SONG:
          mp3.Previous();
          break;
        case Command::SET_VOLUME:
          mp3.V_set(c._data[0]);
          break;
        case Command::MOVE:
          /* In data[0] è contenuta la potenza da dare al motore DX
           * In data[1] quella per il motore SX
           * Il dato è strutturato in modo da contenere nel LSB la direzione (1 = avanti, 0 = indietro)
           * mentre nei successivi bit la potenza effettiva (che va quindi da 0 a 255 a passo di 2)
           */
          // Recupera la direzione in cui mandare i motori
          bool dirX = c._data[0] & 0x01,
               dirY = c._data[1] & 0x01;
          // Imposta la direzione del motore DX
          digitalWrite(PIN_DX_DIR1, dirX);
          digitalWrite(PIN_DX_DIR2, !dirX);
          // Imposta la direzione del motore DX
          digitalWrite(PIN_SX_DIR1, dirY);
          digitalWrite(PIN_SX_DIR2, !dirY);
          // Imposta la potenza data ai motori (0xFE = 11111110B -> rende sempre zero il LSB)
          analogWrite(PIN_MOTORE_DX, c._data[0] & 0xFE);
          analogWrite(PIN_MOTORE_SX, c._data[1] & 0xFE);
        break;
      }
    }
  }
}
