// Link importanti: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
// NOTE: Per vedere le pagine vai su "http://192.168.4.1/" quando sei connesso all'AP

// ESP come Access Point

// Librerie per il funzionamento
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define MYSSID "Roombarmato"
#define MYPASSWORD "e1m1-2077"
#define port 80 // Porta di default per i web server
#define PIN_LED 2

// Oggetto per la gestione del server
ESP8266WebServer server(port);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  // Durante il boot il led è acceso, si spegne a boot completato
  digitalWrite(PIN_LED, LOW);
  
  Serial.begin(74880);
  Serial.println("");
  Serial.println("Booting up...");
  
  WiFi.softAP(MYSSID, MYPASSWORD); // Creazione dell'AP (togliendo il parametro PW l'AP diventa pubblico
  Serial.print("AP created!\n\t> SSID: ");
  Serial.println(MYSSID);
  Serial.print("\t> PW: ");
  Serial.println(MYPASSWORD);
  Serial.print("\t> IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Riempi il server di pagine e funzioni
  creaPagine();
  server.onNotFound(paginaNonTrovato);

  // Fai partire il server
  server.begin();
  Serial.println("HTTP server started!");

  digitalWrite(PIN_LED, HIGH);
}

void loop() {
  // Gestisci il server
  server.handleClient();

  // Se dovesse arrivare un comando dalla seriale
  if(Serial.available()>0) {
    digitalWrite(PIN_LED, LOW);
    delay(50);
    digitalWrite(PIN_LED, HIGH);
    
    String messaggio = Serial.readStringUntil('\n');
    if(messaggio.startsWith(">")){
      messaggio = messaggio.substring(1);
      eseguiComando(messaggio);
    }
  }
}
