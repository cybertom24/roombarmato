#ifndef CustomBuffer_H
  #define CustomBuffer_H
  #include "arduino.h"
  #include "Command.h"
  #define BUFFER_SIZE Command::SIZE
  
class CustomBuffer {
  public:
    CustomBuffer(byte _buffer[BUFFER_SIZE]);
    byte buffer[BUFFER_SIZE];
  private:
};
#endif
