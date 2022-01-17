#include "chip8_emulator/Chip8.h"

#include <chip8_emulator/utils.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <cassert>

namespace ch8
{
    // ROM instructions start at a specific address
    constexpr unsigned int MEMORY_START_ADDRESS = 0x200;

    constexpr unsigned int FONTSET_START_ADDRESS = 0x50;

    constexpr auto FONTSET = ch8::make_array<uint8_t>(
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
    );
}

#ifdef DEBUG
// Static seed in Debug
#define SEED() 0
#else
// Random seed in Release
#define SEED() std::random_device{}()
#endif


ch8::Chip8::Chip8() :
        _pc(MEMORY_START_ADDRESS),
        _randomEngine(SEED()),
        _randByte(0u, std::numeric_limits<uint8_t>::max())
{
    // Load Fonts in memory
    std::copy(FONTSET.cbegin(), FONTSET.cend(), _memory.begin() + FONTSET_START_ADDRESS);
}

bool ch8::Chip8::loadROM(std::string_view filePath)
{
    // Open the file as a stream of binary and move the file pointer to the end
    std::ifstream file(filePath.data(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to read ROM file at location \"" << filePath << '"';
        return false;
    }
    // Get file's size and allocate a buffer to hold the contents
    const std::streamsize buffer_size = file.tellg();
    if (buffer_size == 0 || buffer_size > std::numeric_limits<unsigned int>::max()) {
        std::cerr << "Invalid size (or file is too big) at location \"" << filePath << '"';
        return false;
    }

    std::vector<char> buffer;
    buffer.reserve(buffer_size);
    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), buffer_size);
    file.close();

    // Load buffer into memory
    std::memcpy(_memory.data() + MEMORY_START_ADDRESS, buffer.data(), buffer_size);
    return true;
}

// Wipe all memory
void ch8::Chip8::resetState() noexcept
{
    std::fill(_memory.begin() + MEMORY_START_ADDRESS, _memory.end(), uint8_t(0));
    _registers.fill(0u);
    _keypad.fill(0u);
    _video.fill(1u);
    _stack.fill(0u);
    _sp = 0u;
    _pc = MEMORY_START_ADDRESS;
    _index = 0u;
    _delayTimer = 0u;
    _soundTimer = 0u;
    _renderFlag = false;
}

std::string ch8::Chip8::opcodeToString() const
{
    std::stringstream ss;
    ss << std::hex << _opcode;
    auto opcodeHex = ss.str();
    return opcodeHex;
}

void ch8::Chip8::execCpuCycle()
{
    // Opcode stored in 2 consecutive bytes
    _opcode = (_memory[_pc] << 8) | _memory[_pc + 1];

#ifdef DEBUG
    _opcodeStr = opcodeToString();
#endif

    // run the current CPU instruction stored in _opcode
    execCurrentInstruction();

    if (_delayTimer > 0u) {
        --_delayTimer;
    }
    if (_soundTimer > 0u) {
        --_soundTimer;
        // TODO request SDL buzzer
    }
}

void ch8::Chip8::execCurrentInstruction()
{
    const auto opcodeFirstChar = (_opcode & 0xF000u) >> 12u;
    switch (opcodeFirstChar) {
        case 0x0: {
            switch (_opcode & 0x000Fu) {
                case 0x0:
                    op_00E0();
                    break;
                case 0xE:
                    op_00EE();
                    break;

                default:
                    assert(false);
                    break;
            }
            break;
        }
        case 0x1:
            op_1nnn();
            break;
        case 0x2:
            op_2nnn();
            break;
        case 0x3:
            op_3xkk();
            break;
        case 0x4:
            op_4xkk();
            break;
        case 0x5:
            op_5xy0();
            break;
        case 0x6:
            op_6xkk();
            break;
        case 0x7:
            op_7xkk();
            break;
        case 0x8: {
            switch (_opcode & 0x000Fu) {
                case 0x0:
                    op_8xy0();
                    break;
                case 0x1:
                    op_8xy1();
                    break;
                case 0x2:
                    op_8xy2();
                    break;
                case 0x3:
                    op_8xy3();
                    break;
                case 0x4:
                    op_8xy4();
                    break;
                case 0x5:
                    op_8xy5();
                    break;
                case 0x6:
                    op_8xy6();
                    break;
                case 0x7:
                    op_8xy7();
                    break;
                case 0xE:
                    op_8xyE();
                    break;

                default:
                    assert(false);
                    break;
            }
            break;
        }
        case 0x9:
            op_9xy0();
            break;
        case 0xA:
            op_Annn();
            break;
        case 0xB:
            op_Bnnn();
            break;
        case 0xC:
            op_Cxkk();
            break;
        case 0xD:
            op_Dxyn();
            break;
        case 0xE: {
            switch (_opcode & 0x000Fu) {
                case 0x1:
                    op_ExA1();
                    break;
                case 0xE:
                    op_Ex9E();
                    break;

                default:
                    assert(false);
                    break;
            }
            break;
        }
        case 0xF: {
            switch (_opcode & 0x00FFu) {
                case 0x07:
                    op_Fx07();
                    break;
                case 0x0A:
                    op_Fx0A();
                    break;
                case 0x15:
                    op_Fx15();
                    break;
                case 0x18:
                    op_Fx18();
                    break;
                case 0x1E:
                    op_Fx1E();
                    break;
                case 0x29:
                    op_Fx29();
                    break;
                case 0x33:
                    op_Fx33();
                    break;
                case 0x55:
                    op_Fx55();
                    break;
                case 0x65:
                    op_Fx65();
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        }

        default: {
            assert(false);
            break;
        }
    }
}

#pragma region OPCODES handlers

// Clear the display
void ch8::Chip8::op_00E0()
{
    _video.fill(0);
    _renderFlag = true;
    _pc += 2;
}

// Return from subroutine
void ch8::Chip8::op_00EE()
{
    --_sp;
    _pc = _stack[_sp];
    _pc += 2;
}

// Jump to location nnn
void ch8::Chip8::op_1nnn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _pc = address;
}

// Call subroutine at nnn
void ch8::Chip8::op_2nnn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _stack[_sp] = _pc;
    ++_sp;
    _pc = address;
}

// Skip next instruction if Vx == kk
void ch8::Chip8::op_3xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    if (_registers[Vx] == byte) {
        // Skip next instruction
        _pc += 2;
    }
    _pc += 2;
}

// Skip next instruction if Vx != kk
void ch8::Chip8::op_4xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    if (_registers[Vx] != byte) {
        _pc += 2;
    }
    _pc += 2;
}

// Skip next instruction if Vx == Vy
void ch8::Chip8::op_5xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    if (_registers[Vx] == _registers[Vy]) {
        _pc += 2;
    }
    _pc += 2;
}

// Set Vx = kk
void ch8::Chip8::op_6xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    _registers[Vx] = byte;
    _pc += 2;
}

// Set Vx = Vx + kk
void ch8::Chip8::op_7xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    _registers[Vx] += byte;
    _pc += 2;
}

// Stores the value of register Vy in register Vx
void ch8::Chip8::op_8xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] = _registers[Vy];
    _pc += 2;
}

// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy1()
{
    const uint8_t Vx = (_opcode & 0x0F00) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0) >> 4u;
    _registers[Vx] |= _registers[Vy];
    _pc += 2;
}

// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy2()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] &= _registers[Vy];
    _pc += 2;
}

// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy3()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] ^= _registers[Vy];
    _pc += 2;
}

// The values of Vx and Vy are added together
// If the result is greater than 8 bits, VF is set to 1, otherwise 0
// Only the lowest 8 bits of the result are kept, and stored in Vx
void ch8::Chip8::op_8xy4()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    const auto sum = _registers[Vx] + _registers[Vy];
    _registers[0xF] = sum > 255u ? 1 : 0;
    _registers[Vx] = sum & 0xFFu;
    _pc += 2;
}

// If Vx > Vy, then VF is set to 1, otherwise 0
// Then Vy is subtracted from Vx, and the results stored in Vx
void ch8::Chip8::op_8xy5()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[0xF] = _registers[Vx] > _registers[Vy] ? 1 : 0;
    _registers[Vx] -= _registers[Vy];
    _pc += 2;
}

// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
// Then Vx is divided by 2
void ch8::Chip8::op_8xy6()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0x1u);
    _registers[Vx] >>= 1;
    _pc += 2;
}

// If Vy > Vx, then VF is set to 1, otherwise 0
// Then Vx is subtracted from Vy, and the results stored in Vx.
void ch8::Chip8::op_8xy7()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[0xF] = _registers[Vy] > _registers[Vx] ? 1 : 0;
    _registers[Vx] = _registers[Vy] - _registers[Vx];
    _pc += 2;
}

// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0
// Then Vx is multiplied by 2
void ch8::Chip8::op_8xyE()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0x80u) >> 7u;
    _registers[Vx] <<= 1;
    _pc += 2;
}

// Skip next instruction if Vx != Vy
void ch8::Chip8::op_9xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _pc += 2;
    if (_registers[Vx] != _registers[Vy]) {
        _pc += 2;
    }
}

// Set I = nnn
void ch8::Chip8::op_Annn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _index = address;
    _pc += 2;
}

// Jump to location nnn + V0
// The program counter is set to nnn plus the value of V0
void ch8::Chip8::op_Bnnn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _pc = address + _registers[0];
    _pc += 2;
}

// Set Vx = random byte AND kk
void ch8::Chip8::op_Cxkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = (_opcode & 0x00FFu);
    _registers[Vx] = _randByte(_randomEngine) & byte;
    _pc += 2;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy)
// Set VF = collision
void ch8::Chip8::op_Dxyn()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    const uint8_t height = _opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    const uint8_t xPos = _registers[Vx] % VIDEO_WIDTH;
    const uint8_t yPos = _registers[Vy] % VIDEO_HEIGHT;

    _registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
        const uint8_t spriteByte = _memory[_index + row];

        for (unsigned int col = 0; col < 8; ++col) {
            const uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t &screenPixel = _video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (!spritePixel) {
                // Sprite pixel is off
                continue;
            }

            if (screenPixel == 0xFFFFFFFFu) {
                // Screen pixel also on - notify collision
                _registers[0xF] = 1;
            }
            // Effectively XOR with the sprite pixel
            screenPixel ^= 0xFFFFFFFFu;
        }
    }
    _renderFlag = true;
    _pc += 2;
}

// Skip next instruction if key with the value of Vx is pressed
void ch8::Chip8::op_Ex9E()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t key = _registers[Vx];
    _pc += 2;
    if (_keypad[key] != 0u) {
        _pc += 2;
    }
}

// Skip next instruction if key with the value of Vx is not pressed
void ch8::Chip8::op_ExA1()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t key = _registers[Vx];
    _pc += 2;
    if (_keypad[key] == 0u) {
        _pc += 2;
    }
}

// Set Vx = delay timer value
void ch8::Chip8::op_Fx07()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[Vx] = _delayTimer;
    _pc += 2;
}

// Wait for a key press, store the value of the key in Vx
void ch8::Chip8::op_Fx0A()
{
    bool keyPressed = false;
    const auto size = (uint8_t) _keypad.size();
    for (uint8_t i = 0u; i < size; ++i) {
        if (_keypad[i]) {
            const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
            _registers[Vx] = i;
            keyPressed = true;
        }
    }

    // Doesn't move to the next instruction until the next keyboard input
    // This has the effect of running the same instruction repeatedly
    if (keyPressed) {
        _pc += 2;
    }
}

// Set delay timer = Vx
void ch8::Chip8::op_Fx15()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _delayTimer = _registers[Vx];
    _pc += 2;
}

// Set sound timer = Vx
void ch8::Chip8::op_Fx18()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _soundTimer = _registers[Vx];
    _pc += 2;
}

// Set I = I + Vx
void ch8::Chip8::op_Fx1E()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _index += _registers[Vx];
    _pc += 2;
}

// Set I = location of sprite for digit Vx
void ch8::Chip8::op_Fx29()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    // Font is 5 bytes wide
    _index = FONTSET_START_ADDRESS + (5 * _registers[Vx]);
    _pc += 2;
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2
void ch8::Chip8::op_Fx33()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    uint8_t value = _registers[Vx];
    // Ones-place
    _memory[_index + 2] = value % 10;
    value /= 10;

    // Tens-place
    _memory[_index + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    _memory[_index] = value % 10;

    _pc += 2;
}

// Store registers V0 through Vx in memory starting at location I
void ch8::Chip8::op_Fx55()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0u; i <= Vx; ++i) {
        _memory[_index + i] = _registers[i];
    }
    _pc += 2;
}

// Read registers V0 through Vx from memory starting at location I
void ch8::Chip8::op_Fx65()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0u; i <= Vx; ++i) {
        _registers[i] = _memory[_index + i];
    }
    _pc += 2;
}

#pragma endregion