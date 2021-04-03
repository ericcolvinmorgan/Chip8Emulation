#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include "cpu.hpp"

//From https://en.wikipedia.org/wiki/Nibble
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b)&0x0F)
#define HI_BYTE(w) (((w) >> 8) & 0x00FF)
#define LO_BYTE(w) ((w)&0x00FF)

const int MEMORY_PROGRAM_START = 0x200;
const int MEMORY_END = 0xfff;

CPU::CPU()
{
    //Represents the base font that will be loaded.
    const BYTE font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    //Load font into memory.
    memcpy(&_memory, font, sizeof(font));

    //Seed random
    std::random_device rd;
    _randomEngine = std::mt19937(rd());
}

void CPU::loadIBM()
{
    const BYTE rom[] = { 0x00, 0xE0, 0xA2, 0x2A, 0x60, 0x0C, 0x61, 0x08, 0xD0, 0x1F, 0x70, 0x09, 0xA2, 0x39, 0xD0, 0x1F, 0xA2, 0x48, 0x70, 0x08, 0xD0, 0x1F, 0x70, 0x04, 0xA2, 0x57, 0xD0, 0x1F, 0x70, 0x08, 0xA2, 0x66, 0xD0, 0x1F, 0x70, 0x08, 0xA2, 0x75, 0xD0, 0x1F, 0x12, 0x28, 0xFF, 0x00, 0xFF, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x38, 0x00, 0x3F, 0x00, 0x3F, 0x00, 0x38, 0x00, 0xFF, 0x00, 0xFF, 0x80, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0x00, 0x80, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0xF8, 0x00, 0xFC, 0x00, 0x3E, 0x00, 0x3F, 0x00, 0x3B, 0x00, 0x39, 0x00, 0xF8, 0x00, 0xF8, 0x03, 0x00, 0x07, 0x00, 0x0F, 0x00, 0xBF, 0x00, 0xFB, 0x00, 0xF3, 0x00, 0xE3, 0x00, 0x43, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xE0, 0x00, 0xE0 };
    
    //Load font into memory.
    memcpy(&_memory[MEMORY_PROGRAM_START], rom, sizeof(rom));

    //Set program counter
    _pc = MEMORY_PROGRAM_START;
}

void CPU::loadROM(const std::string &filePath)
{
    // Open the stream
    std::ifstream inputFile(filePath, std::ios::binary);

    // Determine the file length
    inputFile.seekg(0, std::ios_base::end);
    std::size_t size = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);

    //Read contents to memory and close file.
    BYTE* romData = new BYTE[size];
    inputFile.read((char *)romData, size);
    loadROM(romData);
    delete[] romData;
    inputFile.close(); 
}

void CPU::loadROM(const BYTE* romData)
{
    //Clear memory and load rom data.
    memset(&_memory[MEMORY_PROGRAM_START], 0, (MEMORY_END - MEMORY_PROGRAM_START) + 1);
    memcpy(&_memory[MEMORY_PROGRAM_START], romData, (MEMORY_END - MEMORY_PROGRAM_START) + 1);

    //Set program counter
    _pc = MEMORY_PROGRAM_START;
}

WORD CPU::fetch()
{
    WORD opcode = {_memory[_pc], _memory[_pc + 1]};
    _pc += 2;
    return opcode;
}

void CPU::execute(WORD opcode)
{
    if(_sp > 12)
    {
        printf("SP - %d\n", _sp);
    }

    //Below switch comments based opcode table as detailed at https://en.wikipedia.org/wiki/CHIP-8
    switch (opcode.highByte & 0xF0)
    {
    case 0x00:
    {
        // 00E0 	Display 	disp_clear() 	Clears the screen.
        if (opcode.highByte == (BYTE)0x00 && opcode.lowByte == (BYTE)0xE0)
        {
            //Clear Screen
            _screenPixels.reset();
        }
        // 00EE 	Flow 	return; 	Returns from a subroutine.
        else if (opcode.highByte == (BYTE)0x00 && opcode.lowByte == (BYTE)0xEE)
        {
            _pc = _stack[--_sp];
        }
        // 0NNN 	Call 		Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
        else
        {
            throw std::logic_error("Not Implemented - 0NNN");
        }
    }
    break;

    // 1NNN 	Flow 	goto NNN; 	Jumps to address NNN.
    case 0x10:
    {
        _pc = ((opcode.highByte & 0x0F) << 8 | (uint8_t)opcode.lowByte);
    }
    break;

    // 2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN.
    case 0x20:
    {
        _stack[_sp++] = _pc;
        _pc = ((opcode.highByte & 0x0F) << 8 | (uint8_t)opcode.lowByte);
    }
    break;

    // 3XNN 	Cond 	if(Vx==NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
    case 0x30:
    {
        if (_v[(opcode.highByte & 0x0F)] == opcode.lowByte)
        {
            _pc += 2;
        }
    }
    break;

    // 4XNN 	Cond 	if(Vx!=NN) 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
    case 0x40:
    {
        if (_v[(opcode.highByte & 0x0F)] != opcode.lowByte)
        {
            _pc += 2;
        }
    }
    break;

    // 5XY0 	Cond 	if(Vx==Vy) 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
    case 0x50:
    {
        if (_v[(opcode.highByte & 0x0F)] == _v[(opcode.lowByte & 0xF0) >> 4])
        {
            _pc += 2;
        }
    }
    break;

    // 6XNN 	Const 	Vx = NN 	Sets VX to NN.
    case 0x60:
    {
        _v[(opcode.highByte & 0x0F)] = opcode.lowByte;
    }
    break;

    // 7XNN 	Const 	Vx += NN 	Adds NN to VX. (Carry flag is not changed)
    case 0x70:
    {
        _v[(opcode.highByte & 0x0F)] += opcode.lowByte;
    }
    break;

    // 8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY.
    // 8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
    // 8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
    // 8XY3[a] 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.
    // 8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
    // 8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    // 8XY6[a] 	BitOp 	Vx>>=1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
    // 8XY7[a] 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    // 8XYE[a] 	BitOp 	Vx<<=1 	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
    case 0x80:
    {
        int x = (opcode.highByte & 0x0F);
        int y = (opcode.lowByte & 0xF0) >> 4;
        switch (opcode.lowByte & 0x0F)
        {
        case 0x00:
            _v[x] = _v[y];
            break;

        case 0x01:
            _v[x] |= _v[y];
            break;

        case 0x02:
            _v[x] &= _v[y];
            break;

        case 0x03:
            _v[x] ^= _v[y];
            break;

        case 0x04:
        {
            uint16_t value = _v[x] + _v[y];

            if ((value & 0xff00) > 0)
            {
                _v[0xf] = 1;
            }
            else
            {
                _v[0xf] = 0;
            }

            _v[x] = (value & 0x00ff);
        }
        break;

        case 0x05:
            if (_v[x] > _v[y])
            {
                _v[0xf] = 1;
            }
            else
            {
                _v[0xf] = 0;
            }
            _v[x] -= _v[y];
            break;

        case 0x06:
            _v[0x0f] = (_v[x] & 0b00000001);
            _v[x] >>= 1;
            break;

        case 0x07:
            if (_v[y] > _v[x])
            {
                _v[0xf] = 1;
            }
            else
            {
                _v[0xf] = 0;
            }
            _v[x] = _v[y] - _v[x];
            break;

        case 0x0E:
            _v[0x0f] = (_v[x] & 0b10000000) >> 7;
            _v[x] <<= 1;
            break;

        default:
            throw std::logic_error("Not Implemented - 8");
            break;
        }
    }
    break;

    // 9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
    case 0x90:
    {
        if (_v[(opcode.highByte & 0x0F)] != _v[(opcode.lowByte & 0xF0) >> 4])
        {
            _pc += 2;
        }
    }
    break;

    // ANNN 	MEM 	I = NNN 	Sets I to the address NNN.
    case 0xA0:
    {
        _i = ((opcode.highByte & 0x0F) << 8 | (uint8_t)opcode.lowByte);
    }
    break;

    // BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.
    case 0xB0:
    {
        _pc = ((opcode.highByte & 0x0F) << 8 | (uint8_t)opcode.lowByte) + _v[0];
    }
    break;

    // CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    case 0xC0:
    {
        std::uniform_int_distribution<uint8_t> randomDistribution(0, 255);
        auto randomNum = randomDistribution(_randomEngine);
        _v[(opcode.highByte & 0x0F)] = randomNum & opcode.lowByte;
    }
    break;

    // DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
    case 0xD0:
    {
        int x = _v[(opcode.highByte & 0x0F)];
        int y = _v[(opcode.lowByte & 0xF0) >> 4];
        _v[0xf] = 0;

        for (int8_t spritePos = 0; spritePos < ((opcode.lowByte & 0x0F)); spritePos++, y++)
        {
            if (y < CHIP_SCREEN_HEIGHT)
            {
                BYTE spriteLine = _memory[_i + spritePos];
                int scanLineStart = (y * CHIP_SCREEN_WIDTH);
                int scanLineEnd = (y * CHIP_SCREEN_WIDTH) + (CHIP_SCREEN_WIDTH - 1);
                int screenPos = (y * CHIP_SCREEN_WIDTH) + x;

                for (int8_t bitPos = 0; bitPos < 8; bitPos++)
                {
                    if(screenPos >= CHIP_SCREEN_HEIGHT * CHIP_SCREEN_WIDTH)
                    {
                        screenPos = CHIP_SCREEN_HEIGHT * CHIP_SCREEN_WIDTH - 1;
                    }
                    
                    bool cVal = getPixel(screenPos);
                    bool nVal = (spriteLine & 0b10000000);
                    if (cVal && nVal)
                    {
                        _v[0xf] = 1;
                        _screenPixels.set(screenPos, 0);
                    }
                    else
                    {
                        _screenPixels.set(screenPos, cVal ^ nVal);
                    }

                    screenPos++;
                    spriteLine <<= 1;

                    if (screenPos > scanLineEnd)
                    {
                        screenPos = scanLineStart;
                    }
                }
            }
        }
    }
    break;

    // EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
    // EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
    case 0xE0:
    {
        int x = _v[(opcode.highByte & 0x0F)];
        switch (opcode.lowByte)
        {
        case (BYTE)0x9E:
            if (_keyPressed[x])
                _pc += 2;
            break;

        case (BYTE)0xA1:
            if (!_keyPressed[x])
                _pc += 2;
            break;

        default:
            throw std::logic_error("Not Implemented - E");
            break;
        }
    }
    break;

    // FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
    // FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
    // FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
    // FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
    // FX1E 	MEM 	I +=Vx 	Adds VX to I. VF is not affected.[c]
    // FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    // FX33 	BCD 	set_BCD(Vx);

    // *(I+0)=BCD(3);

    // *(I+1)=BCD(2);

    // *(I+2)=BCD(1);
    // 	Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
    // FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
    // FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
    case 0xF0:
    {
        uint8_t x = (opcode.highByte & 0x0F);
        switch (opcode.lowByte)
        {
        case (BYTE)0x07:
            _v[x] = _dt;
            break;

        case (BYTE)0x0A:
            _pc -= 2;
            for (int8_t key = 0; key < 0xf + 1; key++)
            {
                if (_keyPressed[key])
                {
                    _pc += 2;
                    break;
                }
            }
            break;

        case (BYTE)0x15:
            _dt = _v[x]; 
            break;

        case (BYTE)0x18:
            _st = _v[x];
            break;

        case (BYTE)0x1E:
            _i += _v[x];
            break;

        case (BYTE)0x29:
            _i = _v[x] * 5;
            break;

        case (BYTE)0x33:
        {
            uint8_t value = _v[x];
            for (int pos = 2; pos >= 0; pos--)
            {
                _memory[_i + pos] = (value % 10);
                value /= 10;
            }
        }
        break;

        case (BYTE)0x55:
            for (uint8_t pos = 0; pos <= x; pos++)
            {
                _memory[_i + pos] = _v[pos];
            }
            break;

        case (BYTE)0x65:
            for (uint8_t pos = 0; pos <= x; pos++)
            {
                _v[pos] = _memory[_i + pos];
            }
            break;

        default:
            throw std::logic_error("Not Implemented - F");
            break;
        }
    }
    break;
    }
}

void CPU::setKeyPressed(uint8_t key, bool pressed)
{
    if (key >= 0x00 && key <= 0x0f)
    {
        _keyPressed[key] = pressed;
    }
}

void CPU::decrementDelayTimer(BYTE step)
{
    _dt -= step;
    if(_dt < 0)
        _dt = 0;
}

void CPU::decrementSoundTimer(BYTE step)
{
    _st -= step;
    if(_st < 0)
        _st = 0;
}