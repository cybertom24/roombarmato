#ifndef MP3Serial_H
#define MP3Serial_H

/* LIBRERIE */
#include "arduino.h"
#include <SoftwareSerial.h>

/* COSTANTI */
#define MP3_START_BYTE 0x7E
#define MP3_END_BYTE 0xEF


class MP3Serial {
  public:
    MP3Serial(int rx,int tx);

    void begin();
    void play();
    void playIndex(int index);
    void pause();
    void next();
    void previous();
    void volumeUp();
    void volumeDown();
    void volumeSet(int volume);
    
  private:
    SoftwareSerial mp3;
    int _rx;
    int _tx;
};

#endif
