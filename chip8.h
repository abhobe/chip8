#pragma once
#include <cstdint>


class Chip8 {


private:
  uint8_t   ram[4096];
  uint8_t   display[64 * 32];
  uint16_t  pc;
  uint16_t  stack[16];
  uint8_t   sp;
  uint16_t  I;
  uint16_t   delayTimer;
  uint16_t   soundTimer;
  uint16_t  V[16];

public:
  Chip8();
  void cycle();
  bool load(const char* filename);
};