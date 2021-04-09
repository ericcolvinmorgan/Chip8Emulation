#pragma once
#include <bitset>
#include <random>
#include <string>
#include "constants.hpp"

class CPU
{
public:
  CPU();
  ~CPU(){};
  void loadROM(const std::string &filePath);
  void loadROM(const BYTE *romData);
  WORD fetch();
  void execute(WORD opcode);
  void reset();
  const std::bitset<CHIP_SCREEN_WIDTH * CHIP_SCREEN_HEIGHT> *const getScreenPixels() { return &_screenPixels; }
  bool getPixel(size_t pos) { return _screenPixels.test(pos); };
  void setKeyPressed(uint8_t key, bool pressed);
  void decrementDelayTimer(BYTE step);
  void decrementSoundTimer(BYTE step);
  const uint16_t *const getStack() { return _stack; }
  const uint8_t *const getStackPointer() { return &_sp; }
  const uint16_t *const getProgramCounter() { return &_pc; }
  const uint16_t *const getIndexRegister() { return &_i; }
  const BYTE *const getDelayTimer() { return &_dt; }
  const BYTE *const getSoundTimer() { return &_st; }
  const BYTE *const getRegisters() { return _v; }

private:
  uint16_t _stack[16] = {0};
  uint8_t _sp = 0;
  uint16_t _pc = 0;       //Program Counter - Points to current instruction in memory.
  uint16_t _i = 0;        //Index Register - Stores memory addresses, only lowest 12 bits used.
  BYTE _dt = 0;           //Delay Timer - Should be decremented at a rate of 60 Hz (60 times per second) until it reaches 0
  BYTE _st = 0;           //Sound Timer - Functions similar to the Delay Timer, but causes a beep as long as it's not 0.
  BYTE _v[0xf + 1] = {0}; //16 8-bit data registers named V0 to VF
  BYTE _memory[0xfff + 1] = {0};
  BYTE _keyPressed[0xf + 1] = {0};
  std::bitset<CHIP_SCREEN_WIDTH * CHIP_SCREEN_HEIGHT> _screenPixels;
  std::mt19937 _randomEngine;
};