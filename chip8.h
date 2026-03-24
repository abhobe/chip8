#pragma once
#include <cstdint>
#include <chrono>
#include <random>

class Chip8 {

private:
  uint8_t   ram[4096];
  bool   display[64 * 32];
  uint16_t  pc;
  uint16_t  stack[16];
  uint8_t   sp;
  uint16_t  I;
  uint16_t  delayTimer;
  uint16_t  soundTimer;
  uint16_t  V[16];
  uint16_t  opcode;
  bool      keys[16];
  bool      shiftInPlace;
  std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

public:
  Chip8();
  void cycle();
  bool load(const char* filename);


  uint8_t rX();
  uint8_t rY();
  uint16_t rNNN();
  uint8_t rNN();
  uint8_t rN();

  void x00E0();
  void x1NNN();

  void x2NNN();
  void x00EE();

  void x3XNN();
  void x4XNN();
  void x5XY0();
  void x9XY0();

  void x6XNN();
  void x7XNN();

  void x8XY0();
  void x8XY1();
  void x8XY2();
  void x8XY3();
  void x8XY4();
  void x8XY5();
  void x8XY6();
  void x8XY7();
  void x8XYE();

  void xANNN();
  void xBNNN();
  void xCXNN();
  void xDXYN();

  void xEX9E();
  void xEXA1();

  void xFX07();
  void xFX0A();
  void xFX15();
  void xFX18();
  void xFX1E();
  void xFX29();
  void xFX33();
  void xFX55();
  void xFX65();
};