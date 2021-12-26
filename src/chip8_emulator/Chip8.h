//
// Created by mathieu on 24/12/2021.
//

#ifndef CHIP_8_EMULATOR_CHIP8_H
#define CHIP_8_EMULATOR_CHIP8_H

#include <array>
#include <cstdint>
#include <random>

namespace ch8
{
    class Chip8
    {
    public:
        static constexpr int VIDEO_WIDTH = 64;
        static constexpr int VIDEO_HEIGHT = 32;

        Chip8();

        bool loadROM(std::string_view filePath);

        // Execute 1 CPU cycle
        void update();

#pragma region OPCODES

        void op_00E0();     // CLS - Clear the display
        void op_00EE();     // RET - Return from subroutine
        void op_1nnn();     // JP addr - Jump to location nnn
        void op_2nnn();     // CALL addr - Call subroutine at nnn

#pragma endregion

    private:
        std::default_random_engine _randomEngine;
        std::uniform_int_distribution<uint16_t> _randByte;  // Generate random value between 0 and 255

    public:
        std::array<uint8_t, 16> _registers{};     // 16 registers
        std::array<uint8_t, 4096> _memory{};      // 4k of RAM
        uint16_t _index{};                        // special register used to store memory addresses for use in operations
        uint16_t _pc{};                           // Program Counter (holds the address of the next instruction to execute

        std::array<uint16_t, 16> _stack{};
        uint8_t _sp{};                            // Stack Pointer (top of the stack)

        uint8_t _delayTimer{};                    // Simple timer
        uint8_t _soundTimer{};

        std::array<uint8_t, 16> _keypad{};        // Represents each keyboard key (pressed or not pressed)
        std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> _video{};   // Display memory
        uint16_t _opcode;
    };
}

#endif //CHIP_8_EMULATOR_CHIP8_H
