#include <SoftwareSerial.h>

#include "MP3Serial.h"


MP3Serial::MP3Serial(int rx,int tx):mp3(rx,tx) {
}







void MP3Serial::Begin() {

  mp3.begin(9600);
  delay(500);

  int buff1[8] = {0x7E, 0xFF, 0x06, 0x09, 0x01, 0x00, 0x02, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff1[i]);
  }
  delay(500);
}




void MP3Serial::Play() {

  int buff2[8] = {0x7E, 0xFF, 0x06, 0x0D, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }

}



void MP3Serial::Pause() {
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x0E, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::Next() {
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x01, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::Previous() {
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x02, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::V_up() {
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x04, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::V_down() {
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x05, 0x01, 0x00, 0x01, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::V_set(int volume) {
   int _volume=map(volume,0,100,0,30);
  
  int buff2[8] = {0x7E, 0xFF, 0x06, 0x06, 0x01, 0x00, _volume, 0xEF};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}
