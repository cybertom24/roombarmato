#define COMMAND_IP "IP"
#define COMMAND_SSID "SSID"
#define COMMAND_PASSWORD "PASSWORD"
#define COMMAND_LED_ON "LED_ON"
#define COMMAND_LED_OFF "LED_OFF"
#define COMMAND_LED_BLINK "LED_BLINK"

void eseguiComando(String comando) {
  Serial.print(comando);
  Serial.print(" > ");
  if (comando == COMMAND_IP) {
    Serial.println(WiFi.softAPIP());
  }
  else if (comando == COMMAND_SSID) {
    Serial.println(MYSSID);
  }
  else if (comando == COMMAND_PASSWORD) {
    Serial.println(MYPASSWORD);
  }
  else if (comando == COMMAND_LED_ON) {
    digitalWrite(PIN_LED, LOW);
    Serial.println("");
  }
  else if (comando == COMMAND_LED_OFF) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("");
  }
  else if (comando == COMMAND_LED_BLINK) {
    digitalWrite(PIN_LED, LOW);
    delay(50);
    digitalWrite(PIN_LED, HIGH);
    Serial.println("");
  }
  else {
    Serial.println("This command doesn't exist");
  }
}
