//
// Created by mathieu on 24/12/2021.
//

#include "Chip8.h"

#include <fstream>

namespace ch8
{
    // ROM instructions start at a specific address
    constexpr unsigned int MEMORY_START_ADDRESS = 0x200;

    constexpr unsigned int FONTSET_START_ADDRESS = 0x50;

    constexpr std::array<uint8_t, 80> FONTSET{
            0xF0, 0x90, 0x90, 0x90, 0xF0,     // 0
            0x20, 0x60, 0x20, 0x20, 0x70,     // 1
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
        // Static constant seed in Debug mode
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
    std::vector<char> buffer(buffer_size);

    // Go back to the beginning of the file and fill the buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), buffer_size);
    file.close();

    std::copy(buffer.cbegin(), buffer.cend(), _memory.begin() + MEMORY_START_ADDRESS);
    return true;
}


void ch8::Chip8::update()
{
    // TODO
}

#pragma region OPCODES

void ch8::Chip8::op_00E0()
{
    _video.fill(0);
}

void ch8::Chip8::op_00EE()
{
    _pc = _stack[_sp];
    --_sp;
}

void ch8::Chip8::op_1nnn()
{
    _pc = _opcode & 0x0FFFu;
}

void ch8::Chip8::op_2nnn()
{
    ++_sp;
    _stack[_sp] = _pc;
    _pc = _opcode & 0x0FFFu;
}

#pragma endregion