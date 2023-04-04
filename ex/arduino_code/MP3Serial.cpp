#include "MP3Serial.h"

MP3Serial::MP3Serial(int rx,int tx):mp3(rx,tx) {
  _tx = tx;
  _rx = rx;
}

void MP3Serial::begin() {
  mp3.begin(9600);
  delay(500);

  mp3.listen();
  int buff1[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x09, 0x01, 0x00, 0x02, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff1[i]);
  }
  delay(500);
}

void MP3Serial::play() {

  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x0D, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::playIndex(int index) {

  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x03, 0x00, 0x00, index, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}


void MP3Serial::pause() {
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x0E, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::next() {
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x01, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::previous() {
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x02, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::volumeUp() {
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x04, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::volumeDown() {
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x05, 0x01, 0x00, 0x01, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}

void MP3Serial::volumeSet(int volume) {
   int _volume=map(volume,0,100,0,30);
  
  int buff2[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x06, 0x01, 0x00, _volume, MP3_END_BYTE};
  for (int i = 0; i < 8; i++) {
    mp3.write(buff2[i]);
  }
}
