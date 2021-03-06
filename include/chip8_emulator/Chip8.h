#ifndef CHIP_8_EMULATOR_CHIP8_H
#define CHIP_8_EMULATOR_CHIP8_H

#include <array>
#include <cstdint>
#include <random>


namespace ch8
{
    class Chip8 final
    {
    public:
        // Chip8 screen size
        static constexpr int VIDEO_WIDTH = 64;
        static constexpr int VIDEO_HEIGHT = 32;

        // Key indexes used for Chip8::_keypad attribute
        enum Key
        {
            Key_INVALID = -1,
            Key_x = 0,
            Key_1 = 1,
            Key_2 = 2,
            Key_3 = 3,
            Key_a = 4,
            Key_z = 5,
            Key_e = 6,
            Key_q = 7,
            Key_s = 8,
            Key_d = 9,
            Key_w = 10,
            Key_c = 11,
            Key_4 = 12,
            Key_r = 13,
            Key_f = 14,
            Key_v = 15,
        };

        Chip8();

        [[nodiscard]] bool renderRequired() const noexcept {return _renderFlag; }

        void setRenderRequired(bool required) noexcept { _renderFlag = required; }

        // Return the current opcode as string (Hexadecimal format)
        [[nodiscard]] std::string opcodeToString() const;

        // Load the binary file in memory
        bool loadROM(const std::wstring& filePath);

        // Reset to default state (clear screen, memory, keypad, ...)
        void resetState() noexcept;

        // Execute 1 CPU cycle
        void execCpuCycle();

#pragma region OPCODES methods
        void op_00E0();     // CLS
        void op_00EE();     // RET
        void op_1nnn();     // JP addr
        void op_2nnn();     // CALL addr
        void op_3xkk();     // SE Vx, Byte
        void op_4xkk();     // SE Vx, Byte
        void op_5xy0();     // SE Vx, Vy
        void op_6xkk();     // LD Vx, byte
        void op_7xkk();     // ADD Vx, byte
        void op_8xy0();     // LD Vx, Vy
        void op_8xy1();     // OR Vx, Vy
        void op_8xy2();     // AND Vx, Vy
        void op_8xy3();     // XOR Vx, Vy
        void op_8xy4();     // ADD Vx, Vy
        void op_8xy5();     // SUB Vx, Vy
        void op_8xy6();     // SHR Vx
        void op_8xy7();     // SUBN Vx, Vy
        void op_8xyE();     // SHL Vx
        void op_9xy0();     // SNE Vx, Vy
        void op_Annn();     // LD I, addr
        void op_Bnnn();     // JP V0, addr
        void op_Cxkk();     // RND Vx, byte
        void op_Dxyn();     // DRW Vx, Vy, nibble
        void op_Ex9E();     // SKP Vx
        void op_ExA1();     // SKNP Vx
        void op_Fx07();     // LD Vx, DT
        void op_Fx0A();     // LD Vx, K
        void op_Fx15();     // LD DT, Vx
        void op_Fx18();     // Set sound timer = Vx
        void op_Fx1E();     // ADD I, Vx
        void op_Fx29();     // LD F, Vx
        void op_Fx33();     // LD B, Vx
        void op_Fx55();     // LD [I], Vx
        void op_Fx65();     // LD Vx, [I]
#pragma endregion

    private:
        void execCurrentInstruction();

    public:
        std::array<uint8_t, 16> _registers{};                       // 16 registers
        std::array<uint8_t, 4096> _memory{};                        // 4k of RAM
        uint16_t _index{};                                          // special register used to store memory addresses for use in operations
        uint16_t _pc{};                                             // Program Counter (holds the address of the next instruction to execute

        std::array<uint16_t, 16> _stack{};
        uint8_t _sp{};                                              // Stack Pointer (top of the stack)

        uint8_t _delayTimer{};                                      // Simple timer
        uint8_t _soundTimer{};

        std::array<uint8_t, 16> _keypad{};                          // Represents each keyboard key (pressed or not pressed)
        std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> _video{};  // Display memory, uint32 for SDL compliance
        uint16_t _opcode {};                                        // current opcode
#ifdef DEBUG
        std::string _opcodeStr {};
#endif
    private:
        std::default_random_engine _randomEngine;
        std::uniform_int_distribution<uint16_t> _randByte;          // Generate random value between 0 and 255

        bool _renderFlag = true;                                    // Indicate when the UI need to be rendered (when modification happened to _video)
    };
}

#endif //CHIP_8_EMULATOR_CHIP8_H
