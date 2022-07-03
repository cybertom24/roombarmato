#include "CustomBuffer.h"

CustomBuffer::CustomBuffer(byte _buffer[BUFFER_SIZE]) {
  for(int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = _buffer[i];
  }
}
