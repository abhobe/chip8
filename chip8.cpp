#include <chip8.h>
#include <cstdio>
#include <cstring>

#define START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x50

const uint8_t FONTSET[16][5] = {
  {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
  {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
  {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
  {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
  {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
  {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
  {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
  {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
  {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
  {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
  {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
  {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
  {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
  {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
  {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
  {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};



Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()), randByte(0, 255) {
  int reg = FONT_START_ADDRESS;
  for (auto& symbol : FONTSET) { 
    for (auto byte : symbol) {
      ram[reg] = byte;
      reg++;
    }
  }

  pc = START_ADDRESS;
}

Chip8::~Chip8() {
}

void Chip8::cycle() {
  opcode = (ram[pc] << 8u) | ram[pc + 1];
  execute();
  if (delayTimer > 0) delayTimer--;
  if (soundTimer > 0) soundTimer--;
  pc += 2;
}


void Chip8::execute() {
  const uint8_t nibble  = (opcode & 0xF000u) >> 12u;
  
  switch (nibble) {
    case 0x0: x00QQ(); break; // 00E0 and 00EE
    case 0x1: x1NNN(); break;
    case 0x2: x2NNN(); break;
    case 0x3: x3XNN(); break;
    case 0x4: x4XNN(); break;
    case 0x5: x5XY0(); break;
    case 0x6: x6XNN(); break;
    case 0x7: x7XNN(); break;
    case 0x8: x8XYQ(); break; // 8XY0-8XYE
    case 0x9: x9XY0(); break;
    case 0xA: xANNN(); break;
    case 0xB: xBNNN(); break;
    case 0xC: xCXNN(); break;
    case 0xD: xDXYN(); break;
    case 0xE: xEXQQ(); break; // EX9E and EXA1
    case 0xF: xFXQQ(); break; // FX07, FX0A, FX15, FX18, FX1E, FX29, FX33, FX55, and FX65
  }
}

bool Chip8::load(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    return false;
  }

  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t* offset_ram =  ram + START_ADDRESS;

  fread(offset_ram, sizeof(uint8_t), filesize, file);
  fclose(file);
  return true;
}

uint8_t Chip8::rX() {
  return (opcode & 0x0F00u) >> 8u;
}

uint8_t Chip8::rY() {
  return (opcode & 0x00F0u) >> 4u;
}

uint16_t Chip8::rNNN() {
  return opcode & 0x0FFFu;
}

uint8_t Chip8::rNN() {
  return opcode & 0x00FFu;
}

uint8_t Chip8::rN() {
  return opcode & 0x000Fu;
}

// 00E0/00EE - Machine language/Clear/Return dispatcher
void Chip8::x00QQ() {
  const uint8_t last = opcode & 0x00FFu;
  switch (last) {
    case 0xE0: x00E0(); break;
    case 0xEE: x00EE(); break;
  }
}

// 00E0 - CLS
void Chip8::x00E0() {
  memset(display, 0, sizeof(display));
}

// 1NNN - JMP addr
void Chip8::x1NNN() {
  const uint16_t address = rNNN();
  pc = address;
}

// 2NNN - CALL addr (enter subroutine)
void Chip8::x2NNN(){
	const uint16_t address = rNNN();
	stack[sp] = pc;
	++sp;
	pc = address;
}

// 00EE - RET (exit subroutine)
void Chip8::x00EE(){
  --sp;
  pc = stack[sp];
}

// 3XNN - SE Vx, byte (skip next instruction if Vx == byte)
void Chip8::x3XNN() {
  const uint8_t x = rX();
  const uint8_t byte = rNN();
  if (V[x] == byte) {
    pc += 2;
  }
}

// 4XNN - SNE Vx, byte (skip next instruction if Vx != byte)
void Chip8::x4XNN() {
  uint8_t x = rX();
  uint8_t byte = rNN();
  if (V[x] != byte) {
    pc += 2;
  }
}

// 5XY0 - SE Vx, Vy (skip next instruction if Vx == Vy)
void Chip8::x5XY0() {
  uint8_t x = rX();
  uint8_t y = rY();
  if (V[x] == V[y]) {
    pc += 2;
  }
}

// 9XY0 - SNE Vx, Vy (skip next instruction if Vx != Vy)
void Chip8::x9XY0() {
  uint8_t x = rX();
  uint8_t y = rY();
  if (V[x] != V[y]) {
    pc += 2;
  }
}

// 6XNN - LD Vx, byte (set Vx = byte)
void Chip8::x6XNN() {
  uint8_t x = rX();
  uint8_t byte = rNN();
  V[x] = byte;
}

// 7XNN - ADD Vx, byte (set Vx += byte)
void Chip8::x7XNN() {
  uint8_t x = rX();
  uint8_t byte = rNN();
  V[x] += byte;
}


void Chip8::x8XYQ() {
  const uint8_t last = opcode & 0x000Fu;
  switch (last) {
    case 0x0: x8XY0(); break;
    case 0x1: x8XY1(); break;
    case 0x2: x8XY2(); break;
    case 0x3: x8XY3(); break;
    case 0x4: x8XY4(); break;
    case 0x5: x8XY5(); break;
    case 0x6: x8XY6(); break;
    case 0x7: x8XY7(); break;
    case 0xE: x8XYE(); break;
  }
}

// 8XY0 - LD Vx, Vy (set Vx = Vy)
void Chip8::x8XY0() {
  uint8_t x = rX();
  uint8_t y = rY();
  V[x] = V[y];
}

// 8XY1 - OR Vx, Vy (set Vx = Vx OR Vy)
void Chip8::x8XY1() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  V[x] |= V[y];
}

// 8XY2 - AND Vx, Vy (set Vx = Vx AND Vy)
void Chip8::x8XY2() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  V[x] &= V[y];
}

// 8XY3 - XOR Vx, Vy (set Vx = Vx XOR Vy)
void Chip8::x8XY3() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  V[x] ^= V[y];
}

// 8XY4 - ADD Vx, Vy (set Vx = Vx + Vy, set VF = carry)
void Chip8::x8XY4() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  uint16_t sum = V[x] + V[y];
  V[0xF] = (sum > 255u) ? 1 : 0; // carry
  V[x] = sum & 0xFFu;
}

// 8XY5 - SUB Vx, Vy (set Vx = Vx - Vy, set VF = NOT borrow)
void Chip8::x8XY5() {
  uint8_t x = rX();
  uint8_t y = rY();
  V[0xF] = (V[x] > V[y]) ? 1 : 0;
  V[x] -= V[y];
}

// 8XY6 - SHR Vx {, Vy} (set Vx = Vx SHR 1)
void Chip8::x8XY6() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  if (!shiftInPlace) V[x] = V[y];
  V[0xF] = V[x] & 0x1u;
  V[x] >>= 1;
}

// 8XY7 - SUBN Vx, Vy (set Vx = Vy - Vx, set VF = NOT borrow)
void Chip8::x8XY7() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  V[0xF] = (V[y] > V[x]) ? 1 : 0;
  V[x] = V[y] - V[x];
}

// 8XYE - SHL Vx {, Vy} (set Vx = Vx SHL 1)
void Chip8::x8XYE() {
  const uint8_t x = rX();
  const uint8_t y = rY();
  if (!shiftInPlace) V[x] = V[y];
  V[0xF] = (V[x] & 0x80u) >> 7u;
  V[x] <<= 1;
}

// ANNN - LD I, addr (set I = addr)
void Chip8::xANNN() {
  const uint16_t address = rNNN();
  I = address;
}

// BNNN - JP V0, addr (jump to location nnn + V0)
void Chip8::xBNNN() {
  const uint16_t address = rNNN();
  pc = address + V[0];
}

// CXNN - RND Vx, byte (set Vx = random byte AND nn)
void Chip8::xCXNN() {
  const uint8_t x = rX();
  const uint8_t byte = rNN();
  V[x] = randByte(randGen) & byte;
}


// DXYN - DRW Vx, Vy, nibble (display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision)
// TODO: review this
void Chip8::xDXYN() {
  const uint8_t x = V[rX()] & 63;
  const uint8_t y = V[rY()] & 31;
  const uint8_t n = rN();
  V[0xF] = 0;

  for (int j = 0; j<n && y+j<32; j++) {
    uint8_t byte = ram[I + j];
    for (int k = 0; k<8 && x+k<64; k++) {
      // 0x80u = 0b10000000u (shift k mask)
      bool pixel = (byte & (0x80u >> k)) != 0;
      uint16_t index = ((y + j) % 32) * 64 + ((x + k) % 64);
      if (pixel && display[index]) V[0xF] = 1; // collision
      display[index] ^= pixel;
    }
  }
}

// EX9E - SKP Vx (skip next instruction if keys[Vx] is pressed)
void Chip8::xEX9E() {
  const uint8_t x = rX();
  if (keys[V[x]]) {
    pc += 2;
  }
}

// EXA1 - SKNP Vx (skip next instruction if keys[Vx] is not pressed)
void Chip8::xEXA1() {
  const uint8_t x = rX();
  if (!keys[V[x]]) {
    pc += 2;
  }
}

// EX9E/EXA1 - Key operations dispatcher
void Chip8::xEXQQ() {
  const uint8_t last = opcode & 0x00FFu;
  switch (last) {
    case 0x9E: xEX9E(); break;
    case 0xA1: xEXA1(); break;
  }
}

// FX07 - LD Vx, DT (set Vx = delay timer value)
void Chip8::xFX07() {
  const uint8_t x = rX();
  V[x] = delayTimer;
}

// FX0A - LD Vx, K (wait for a key press, store the value of the key in Vx)
void Chip8::xFX0A() {
  const uint8_t x = rX();
  for (int i = 0; i < 16; i++) {
    if (keys[i]) { 
      V[x] = i;
      return;
    }
  }
  pc -= 2; // repeat this instruction until a key is pressed
}

// FX15 - LD DT, Vx (set delay timer = Vx)
void Chip8::xFX15() {
  const uint8_t x = rX();
  delayTimer = V[x];
}

// FX18 - LD ST, Vx (set sound timer = Vx)
void Chip8::xFX18() {
  const uint8_t x = rX();
  soundTimer = V[x];
}

// FX1E - ADD I, Vx (set I = I + Vx)
void Chip8::xFX1E() {
  const uint8_t x = rX();
  const uint16_t sum = I + V[x];
  V[0xF] = (sum > 255) ? 1 : 0; // carry
  I = sum & 0xFFu;
}

// FX29 - LD F, Vx (set I = location of sprite for digit Vx)
void Chip8::xFX29() {
  const uint8_t x = rX();
  I = V[x] * 5 + FONT_START_ADDRESS; // each font symbol is 5 bytes, and fontset starts at FONT_START_ADDRESS
}

// FX33 - LD B, Vx (store BCD representation of Vx in memory locations I, I+1, and I+2)
void Chip8::xFX33() {
  const uint8_t x = rX();
  uint8_t value = V[x];
  ram[I + 2] = value % 10;
  value /= 10;
  ram[I + 1] = value % 10;
  value /= 10;
  ram[I] = value % 10;
}

// FX55 - LD [I], Vx (store registers V0 through Vx in memory starting at location I)
void Chip8::xFX55() {
  const uint8_t x = rX();
  for (int i = 0; i <= x; i++) {
    ram[I + i] = V[i];
  }
}

// FX65 - LD Vx, [I] (read registers V0 through Vx from memory starting at location I)
void Chip8::xFX65() {
  const uint8_t x = rX();
  for (int i = 0; i <= x; i++) {
    V[i] = ram[I + i];
  }
}

// FX07-FX65 - Various F instructions dispatcher
void Chip8::xFXQQ() {
  const uint8_t last = opcode & 0x00FFu;
  switch (last) {
    case 0x07: xFX07(); break;
    case 0x0A: xFX0A(); break;
    case 0x15: xFX15(); break;
    case 0x18: xFX18(); break;
    case 0x1E: xFX1E(); break;
    case 0x29: xFX29(); break;
    case 0x33: xFX33(); break;
    case 0x55: xFX55(); break;
    case 0x65: xFX65(); break;
  }
}