#include "chip8_emulator/Chip8.h"

#include <fstream>
#include <cassert>
#include <iostream>

namespace ch8
{
    // ROM instructions start at a specific address
    constexpr unsigned int MEMORY_START_ADDRESS = 0x200;

    constexpr unsigned int FONTSET_START_ADDRESS = 0x50;

    constexpr std::array<uint8_t, 80> FONTSET{
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
}


ch8::Chip8::Chip8() :
        _pc(MEMORY_START_ADDRESS),
#ifdef DEBUG
        // Static seed in Debug mode
        _randomEngine(0),
#else
        // In release, set a random Seed
        _randomEngine(std::random_device().operator()()),
#endif
        _randByte(0u, std::numeric_limits<uint8_t>::max())
{
    // Load Fonts in memory
    std::copy(FONTSET.cbegin(), FONTSET.cend(), _memory.begin() + FONTSET_START_ADDRESS);
}

bool ch8::Chip8::loadROM(std::string_view filePath)
{
    // Open the file as a stream of binary and move the file pointer to the end
    std::ifstream file(filePath.data(), std::ios::binary | std::ios::ate);

    if (!file.is_open())
        return false;

    // Get size of file and allocate a buffer to hold the contents
    std::streampos buffer_size = file.tellg();
    std::vector<char> buffer;
    buffer.resize(buffer_size);

    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), buffer_size);
    file.close();

    std::memcpy(reinterpret_cast<char *>(&*_memory.begin() + MEMORY_START_ADDRESS), buffer.data(), buffer_size);
    return true;
}

void ch8::Chip8::reset() noexcept
{
    std::fill(_memory.begin() + MEMORY_START_ADDRESS, _memory.end(), uint8_t(0u));
    _registers.fill(0u);
    _keypad.fill(0u);
    _video.fill(0u);
    _stack.fill(0u);
    _pc = MEMORY_START_ADDRESS;
    _sp = 0u;
    _delayTimer = 0u;
    _soundTimer = 0u;
}

void ch8::Chip8::execCpuCycle()
{
    _opcode = (_memory[_pc] << 8) | _memory[_pc + 1];
    _pc += 2;

    execCurrentInstruction();

    if (_delayTimer > 0u) {
        --_delayTimer;
    }
    if (_soundTimer > 0u) {
        --_soundTimer;
    }
}

void ch8::Chip8::debugOpcode() const
{
    const uint16_t x = (_opcode & 0x0F00) >> 8;  // second 4 bits e.g. 0xA(B)CD
    const uint16_t y = (_opcode & 0x00F0) >> 4;  // third 4 bits e.g. 0xAB(C)D
    const uint16_t kk = _opcode & 0x00FF;        // lower byte e.g. 0xAB(CD)
    const uint16_t n = _opcode & 0x000F;         // last 4 bits e.g. 0xABC(D)
    std::cout << _opcode;
}

void ch8::Chip8::execCurrentInstruction()
{
    const auto opcodeFirstChar = (_opcode & 0xF000u) >> 12u;
    switch (opcodeFirstChar) {
        case 0x0: {
            switch (_opcode & 0x000Fu) {
                case 0x0: op_00E0(); break;
                case 0xE: op_00EE(); break;

                default: debugOpcode(); break;
            }
            break;
        }
        case 0x1: op_1nnn();
            break;
        case 0x2: op_2nnn();
            break;
        case 0x3: op_3xkk();
            break;
        case 0x4: op_4xkk();
            break;
        case 0x5: op_5xy0();
            break;
        case 0x6: op_6xkk();
            break;
        case 0x7: op_7xkk();
            break;
        case 0x8: {
            switch (_opcode & 0x000Fu) {
                case 0x0: op_8xy0();
                    break;
                case 0x1: op_8xy1();
                    break;
                case 0x2: op_8xy2();
                    break;
                case 0x3: op_8xy3();
                    break;
                case 0x4: op_8xy4();
                    break;
                case 0x5: op_8xy5();
                    break;
                case 0x6: op_8xy6();
                    break;
                case 0x7: op_8xy7();
                    break;
                case 0xE: op_8xyE();
                    break;

                default: debugOpcode(); break;
            }
            break;
        }
        case 0x9: op_9xy0();
            break;
        case 0xA: op_Annn();
            break;
        case 0xB: op_Bnnn();
            break;
        case 0xC: op_Cxkk();
            break;
        case 0xD: op_Dxyn();
            break;
        case 0xE: {
            switch (_opcode & 0x000Fu) {
                case 0x1: op_ExA1();
                    break;
                case 0xE: op_Ex9E();
                    break;
            }
            debugOpcode();
            break;
        }
        case 0xF: {
            switch (_opcode & 0x000Fu) {
                case 0x3: op_Fx33(); break;
                case 0x5: {
                    switch ((_opcode & 0xF0u) >> 4) {
                        case 0x1: op_Fx15(); break;
                        case 0x5: op_Fx55(); break;
                        case 0x6: op_Fx65(); break;
                    }
                    debugOpcode();
                    break;
                }
                case 0x7: op_Fx07(); break;
                case 0x8: op_Fx18(); break;
                case 0x9: op_Fx29(); break;
                case 0xA: op_Fx0A(); break;
                case 0xE: op_Fx1E(); break;
            }
            debugOpcode();
            break;
        }

        default: {
            debugOpcode();
            break;
        }
    }
}

#pragma region OPCODES handlers

// Clear the display
void ch8::Chip8::op_00E0()
{
    _video.fill(0);
}

// Return from subroutine
void ch8::Chip8::op_00EE()
{
    --_sp;
    _pc = _stack[_sp];
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
        _pc += 2;
    }
}

// Skip next instruction if Vx != kk
void ch8::Chip8::op_4xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    if (_registers[Vx] != byte) {
        _pc += 2;
    }
}

// Skip next instruction if Vx == Vy
void ch8::Chip8::op_5xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    if (_registers[Vx] == _registers[Vy]) {
        _pc += 2;
    }
}

// Set Vx = kk
void ch8::Chip8::op_6xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    _registers[Vx] = byte;
}

// Set Vx = Vx + kk
void ch8::Chip8::op_7xkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = _opcode & 0x00FFu;
    _registers[Vx] += byte;
}

// Stores the value of register Vy in register Vx
void ch8::Chip8::op_8xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] = _registers[Vy];
}

// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy1()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] |= Vy;
}

// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy2()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] &= Vy;
}

// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy3()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[Vx] ^= Vy;
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
}

// If Vx > Vy, then VF is set to 1, otherwise 0
// Then Vy is subtracted from Vx, and the results stored in Vx
void ch8::Chip8::op_8xy5()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[0xF] = _registers[Vx] > _registers[Vy] ? 1 : 0;
    _registers[Vx] -= _registers[Vy];
}

// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
// Then Vx is divided by 2
void ch8::Chip8::op_8xy6()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0x1u);
    _registers[Vx] >>= 1;
}

// If Vy > Vx, then VF is set to 1, otherwise 0
// Then Vx is subtracted from Vy, and the results stored in Vx.
void ch8::Chip8::op_8xy7()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    _registers[0xF] = _registers[Vy] > _registers[Vx] ? 1 : 0;
    _registers[Vx] = _registers[Vy] - _registers[Vx];
}

// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0
// Then Vx is multiplied by 2
void ch8::Chip8::op_8xyE()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0x80u) >> 7u;
    _registers[Vx] <<= 1;

}

// Skip next instruction if Vx != Vy
void ch8::Chip8::op_9xy0()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    if (_registers[Vx] != _registers[Vy]) {
        _pc += 2;
    }
}

// Set I = nnn
void ch8::Chip8::op_Annn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _index = address;
}

// Jump to location nnn + V0
// The program counter is set to nnn plus the value of V0
void ch8::Chip8::op_Bnnn()
{
    const uint16_t address = _opcode & 0x0FFFu;
    _pc = address + _registers[0];
}

// Set Vx = random byte AND kk
void ch8::Chip8::op_Cxkk()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t byte = (_opcode & 0x00FFu);
    _registers[Vx] = _randByte(_randomEngine) & byte;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy)
// Set VF = collision
void ch8::Chip8::op_Dxyn()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t Vy = (_opcode & 0x00F0u) >> 4u;
    const auto nByte = (_opcode & 0x000Fu);

    // If coordinates are outside of the screen, wrap around to the opposite side of the screen
    const auto xPos = _registers[Vx] % VIDEO_WIDTH;
    const auto yPos = _registers[Vy] % VIDEO_HEIGHT;

    _registers[0xF] = 0;

    for (auto row = 0u; row < nByte; ++row) {
        const auto spriteByte = _memory[_index + row];

        for (auto col = 0u; col < 8u; ++col) {
            const uint8_t spritePixel = spriteByte & (0x80u >> col);
            auto &screenPixel = _video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (spritePixel) {
                if (screenPixel == 0xFFFFFFFF) {
                    // Screen pixel already ON, collision
                    _registers[0xF] = 1;
                }
            }
            // XOR with the sprite pixel
            screenPixel ^= 0xFFFFFFFF;
        }
    }
}

// Skip next instruction if key with the value of Vx is pressed
void ch8::Chip8::op_Ex9E()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t key = _registers[Vx];
    if (_keypad[key]) {
        _pc += 2;
    }
}

// Skip next instruction if key with the value of Vx is not pressed
void ch8::Chip8::op_ExA1()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t key = _registers[Vx];
    if (!_keypad[key]) {
        _pc += 2;
    }
}

// Set Vx = delay timer value
void ch8::Chip8::op_Fx07()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _registers[Vx] = _delayTimer;
}

// Wait for a key press, store the value of the key in Vx
void ch8::Chip8::op_Fx0A()
{
    const auto size = (uint8_t) _keypad.size();
    for (uint8_t i = 0u; i < size; ++i) {
        if (_keypad[i]) {
            const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
            _registers[Vx] = i;
            return;
        }
        // The easiest way to “wait” is to decrement the PC by 2 whenever a keypad value is not detected
        // This has the effect of running the same instruction repeatedly.
        _pc -= 2;
    }
}

// Set delay timer = Vx
void ch8::Chip8::op_Fx15()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _delayTimer = _registers[Vx];
}

// Set sound timer = Vx
void ch8::Chip8::op_Fx18()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _soundTimer = _registers[Vx];
}

// Set I = I + Vx
void ch8::Chip8::op_Fx1E()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    _index += _registers[Vx];
}

// Set I = location of sprite for digit Vx
void ch8::Chip8::op_Fx29()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    const uint8_t digit = _registers[Vx];
    // Font is 5 bytes wide
    _index = FONTSET_START_ADDRESS + (5 * digit);
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
}

// Store registers V0 through Vx in memory starting at location I
void ch8::Chip8::op_Fx55()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0u; i <= Vx; ++i) {
        _memory[_index + i] = _registers[i];
    }
}

// Read registers V0 through Vx from memory starting at location I
void ch8::Chip8::op_Fx65()
{
    const uint8_t Vx = (_opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0u; i <= Vx; ++i) {
        _registers[i] = _memory[_index + i];
    }
}

#pragma endregion