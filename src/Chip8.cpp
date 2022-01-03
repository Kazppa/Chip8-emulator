#include "chip8_emulator/Chip8.h"

#include <fstream>

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
    // Open file as a stream binary and move the pointer to the end
    std::ifstream file(filePath.data(), std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return false;

    // Allocate a buffer for the file content
    const auto buffer_size = file.tellg();
    if (buffer_size <= 0) {
        return false;
    }
    std::vector<char> buffer;
    buffer.reserve(buffer_size);

    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), buffer_size);
    file.close();

    std::copy(buffer.cbegin(), buffer.cend(), _memory.begin() + MEMORY_START_ADDRESS);
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

void ch8::Chip8::update()
{
    // TODO
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
    _pc = _stack[_sp];
    --_sp;
}

// Jump to location nnn
void ch8::Chip8::op_1nnn()
{
    _pc = _opcode & 0b00001111'11111111u;
}

// Call subroutine at nnn
void ch8::Chip8::op_2nnn()
{
    ++_sp;
    _stack[_sp] = _pc;
    _pc = _opcode & 0b00001111'11111111u;
}

// Skip next instruction if Vx == kk
void ch8::Chip8::op_3xkk()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const uint8_t byte = _opcode & 0b00000000'11111111u;
    if (_registers[Vx] == byte) {
        _pc += 2;
    }
}

// Skip next instruction if Vx != kk
void ch8::Chip8::op_4xkk()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const uint8_t byte = _opcode & 0b00000000'11111111u;
    if (_registers[Vx] != byte) {
        _pc += 2;
    }
}

// Skip next instruction if Vx == Vy
void ch8::Chip8::op_5xy0()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    if (_registers[Vx] == _registers[Vy]) {
        _pc += 2;
    }
}

// Set Vx = kk
void ch8::Chip8::op_6xkk()
{
    const auto Vx = (_opcode & 0b00001111'0000u) >> 8u;
    const uint8_t byte = _opcode & 0b00000000'11111111u;
    _registers[Vx] = byte;
}

// Set Vx = Vx + kk
void ch8::Chip8::op_7xkk()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const uint8_t byte = _opcode & 0b00000000'11111111u;
    _registers[Vx] += byte;
}

// Stores the value of register Vy in register Vx
void ch8::Chip8::op_8xy0()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[Vx] = _registers[Vy];
}

// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy1()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[Vx] |= Vy;
}

// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy2()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[Vx] &= Vy;
}

// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx
void ch8::Chip8::op_8xy3()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[Vx] ^= Vy;
}

// The values of Vx and Vy are added together
// If the result is greater than 8 bits, VF is set to 1, otherwise 0
// Only the lowest 8 bits of the result are kept, and stored in Vx
void ch8::Chip8::op_8xy4()
{
    const auto Vx = (_opcode & 0b00001111'00000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    const auto sum = _registers[Vx] + _registers[Vy];
    _registers[0xF] = sum > 255u ? 1 : 0;
    _registers[Vx] = sum & 0b00000000'11111111u;
}

// If Vx > Vy, then VF is set to 1, otherwise 0
// Then Vy is subtracted from Vx, and the results stored in Vx
void ch8::Chip8::op_8xy5()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[0xF] = _registers[Vx] > _registers[Vy] ? 1 : 0;
    _registers[Vy] -= _registers[Vx];
}

// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
// Then Vx is divided by 2
void ch8::Chip8::op_8xy6()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0b00000001u);
    _registers[Vx] >>= 1;
}

// If Vy > Vx, then VF is set to 1, otherwise 0
// Then Vx is subtracted from Vy, and the results stored in Vx.
void ch8::Chip8::op_8xy7()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    _registers[0xF] = _registers[Vy] > _registers[Vx] ? 1 : 0;
    _registers[Vx] = _registers[Vy] - _registers[Vx];
}

// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0
// Then Vx is multiplied by 2
void ch8::Chip8::op_8xyE()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    _registers[0xF] = (_registers[Vx] & 0b10000000u) >> 7u;
    _registers[Vx] <<= 1;

}

// Skip next instruction if Vx != Vy
void ch8::Chip8::op_9xy0()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    if (_registers[Vx] != _registers[Vy]) {
        _pc += 2;
    }
}

// Set I = nnn
void ch8::Chip8::op_Annn()
{
    const uint8_t address = _opcode & 0b0000111111111111u;
    _index = address;
}

// Jump to location nnn + V0
// The program counter is set to nnn plus the value of V0
void ch8::Chip8::op_Rnnn()
{
    const uint8_t address = _opcode & 0b0000111111111111u;
    _pc = address + _registers[0];
}

// Set Vx = random byte AND kk
void ch8::Chip8::op_Cxkk()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    const uint8_t byte = (_opcode & 0b00000000'11111111u);
    _registers[Vx] = _randByte(_randByte) & byte;
}

// Display n-byte sprite starting at memory location I at (Vx, Vy)
// Set VF = collision
void ch8::Chip8::op_Dxyn()
{
    const auto Vx = (_opcode & 0b00001111'000000000u) >> 8u;
    const auto Vy = (_opcode & 0b00000000'11110000u) >> 4u;
    const auto nByte = (_opcode & 0b00001111u);

    // If coordinates are outside of the screen, wrap around to the opposite side of the screen
    const auto xPos = _registers[Vx] % VIDEO_WIDTH;
    const auto yPos = _registers[Vy] % VIDEO_HEIGHT;

    _registers[0xF] = 0u;
    for (auto row = 0u; row < nByte; ++row) {
        const auto spriteByte = _memory[_index + row];

        for (auto col = 0u; col < 8u; ++col) {
            const auto spritePixel = spriteByte & (0b10000000u >> col);
            auto& screenPixel = _video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (spritePixel) {
                if (screenPixel == 0b11111111'11111111u) {
                    // Screen pixel already ON, collision
                    _registers[0xF] = 1;
                }
            }
            // XOR with the sprite pixel
            screenPixel ^= 0b11111111'11111111u;
        }
    }
}

#pragma endregion