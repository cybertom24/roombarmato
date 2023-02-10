#include <SoftwareSerial.h>
#define MY_RX 2
#define MY_TX 3
SoftwareSerial seriale(MY_RX, MY_TX);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  seriale.begin(74880);
}

void loop() {
  if(seriale.available()) {
    Serial.print(seriale.readString());
  }
  if(Serial.available()) {
    seriale.print(Serial.readString());
  }
}
