/* LIBRERIE */
#include "UdpConnection.h"
#include "Command.h"
#include "CustomBuffer.h"

/* COSTANTI */
#define PIN_LED 2
#define SSID "Roombarmato"
#define PASSWORD "e1m1-2077"
#define PORT 4000

UdpConnection conn(SSID, PASSWORD, PORT);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  // Durante il boot il led è acceso, si spegne a boot completato
  digitalWrite(PIN_LED, LOW);
  
  conn.setup();
  
  digitalWrite(PIN_LED, HIGH);
}

void loop() {
  if(Serial.available()) {
    digitalWrite(PIN_LED, LOW);

    byte message[COMMAND_SIZE];
    Serial.readBytes(message, COMMAND_SIZE);
    conn.send(message);
    
    digitalWrite(PIN_LED, HIGH);
  } 
  // Controlla se ci sono pacchetti arrivati
  conn.checkPackets();
  // Se ce ne sono di nuovi
  if(conn.newMessage) {
    digitalWrite(PIN_LED, LOW);
    
    CustomBuffer b = conn.getMessage();
    for(int i = 0; i < COMMAND_SIZE; i++) {
      Serial.write(b.buffer[i]);
    }
    conn.refresh();
    
    digitalWrite(PIN_LED, HIGH);
  }
}
