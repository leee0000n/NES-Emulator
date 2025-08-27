#pragma once

using Byte = unsigned char;
using Word = unsigned short;


// CPU constants
constexpr bool HI_BYTE_ENABLED = true;
constexpr bool HI_BYTE_DISABLED = false;

constexpr int CARRY = 1;
constexpr int ZERO = 2;
constexpr int INTERRUPT_DISABLE = 4;
constexpr int DECIMAL = 8;
constexpr int BREAK = 16;
constexpr int NONSENSE_FLAG = 32;
constexpr int OVRFLOW = 64;
constexpr int NEGATIVE = 128;

constexpr int ACCUMULATOR = 1;
constexpr int X_REGISTER = 2;
constexpr int Y_REGISTER = 3;


// PPU constants
constexpr bool FIRST_WRITE = true;
constexpr bool SECOND_WRITE = false;

constexpr int VBLANK = 0x80;

constexpr int BACKGROUND_RENDERING = 0x10;
constexpr int SPRITE_RENDERING = 0x08;

constexpr int COARSE_X_MASK = 0x001F;
constexpr int COARSE_Y_MASK = 0x03E0;
constexpr int FINE_Y_MASK = 0x7000;


// APU constants
constexpr int APU_STATUS_REGISTER_ADDRESS = 0x4015;