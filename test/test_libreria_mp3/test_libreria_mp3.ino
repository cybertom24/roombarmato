#include "MP3Serial.h"
MP3Serial my_mp3(10,4);
String cmd;
String data;
int _data;
void setup() {
  Serial.begin(9600);
  my_mp3.Begin();
}

void loop() {
  
}
