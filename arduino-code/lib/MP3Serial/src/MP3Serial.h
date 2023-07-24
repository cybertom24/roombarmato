#ifndef MP3Serial_H
#define MP3Serial_H

/* LIBRERIE */
#include "Arduino.h"
// #include <SoftwareSerial.h>

/* COSTANTI */
#define MP3_START_BYTE 0x7E
#define MP3_END_BYTE 0xEF
#define SERIAL_BAUD_RATE 9600
#define MP3_COMMAND_SIZE 8
#define DELAY_INIT 500

class MP3Serial
{
public:
    // Modifica: ora il modulo mp3 usa la seriale di default
    /* MP3Serial(int rx, int tx) : mp3(rx, tx)
    {
        _tx = tx;
        _rx = rx;
    }; */

    MP3Serial()
    {
        // Vuota
    }

    void begin()
    {
        Serial.begin(SERIAL_BAUD_RATE);
        while(!Serial)
            ;   // Aspetta nel mentre che si inizializza
        delay(DELAY_INIT);

        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x09, 0x01, 0x00, 0x02, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
        delay(DELAY_INIT);
    };

    void play()
    {

        byte buff[8] = {MP3_START_BYTE, 0xFF, 0x06, 0x0D, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void playIndex(byte index)
    {

        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x03, 0x00, 0x00, index, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void pause()
    {
        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x0E, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void next()
    {
        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x01, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void previous()
    {
        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x02, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void volumeUp()
    {
        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x04, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

    void volumeDown()
    {
        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x05, 0x01, 0x00, 0x01, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };
    
    void volumeSet(int volume)
    {
        byte _volume = map(volume, 0, 100, 0, 30);

        byte buff[MP3_COMMAND_SIZE] = {MP3_START_BYTE, 0xFF, 0x06, 0x06, 0x01, 0x00, _volume, MP3_END_BYTE};
        Serial.write(buff, MP3_COMMAND_SIZE);
    };

private:
    /* SoftwareSerial mp3;
    int _rx;
    int _tx; */
};

#endif
