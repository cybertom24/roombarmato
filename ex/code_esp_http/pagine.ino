void creaPagine() {
  server.on("/", mainPagina);
  server.on("/test", testPagina);
  server.on("/gif", gifPagina);
}

void mainPagina() {
  // Quando qualcuno si collega a "http://ip/"
  Serial.println("Someone has entered \"/\"");
  server.send(200, "text/plain", "Ciao bello, hai visto che funziono?");
  digitalWrite(PIN_LED, LOW);
  delay(100);
  digitalWrite(PIN_LED, HIGH);
}

void testPagina() {
  // Quando qualcuno si collega a "http://ip/test"
  Serial.println("Someone has entered \"/test\"");
  server.send(200, "text/plain", "Oh, hai scoperto un altra pagina");
}

void gifPagina() {
  // Quando qualcuno si collega a "http://ip/gif"
  Serial.println("Someone has entered \"/gif\"");
  
  static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
}

void paginaNonTrovato() {
  // Se entra in una pagina non esistente lancia il messaggio Err404
  Serial.println("Someone tried to use an illegal link");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
