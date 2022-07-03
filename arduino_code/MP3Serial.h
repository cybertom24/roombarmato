#ifndef MP3Serial_H
#define MP3Serial_H
#include "arduino.h"
#include <SoftwareSerial.h>

class MP3Serial {
  public:
    MP3Serial(int rx,int tx);
    int _rx;
    int _tx;
    void Begin();
    void Play();
    void Play_index(int index);
    void Pause();
    void Next();
    void Previous();
    void V_up();
    void V_down();
    void V_set(int volume);
    void P_folder1();
    void P_folder2();
    void P_folder3();
    void P_folder4();
    void P_folder5();
    void Sleep();
    void Wake_up();
    void Reset();
    private:
    SoftwareSerial mp3;
};

#endif
