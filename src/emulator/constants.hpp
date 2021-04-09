#pragma once

#include <stdint.h>

typedef uint8_t BYTE;

typedef struct
{
  BYTE highByte;
  BYTE lowByte;
} WORD;

const short CHIP_SCREEN_WIDTH = 64;
const short CHIP_SCREEN_HEIGHT = 32;