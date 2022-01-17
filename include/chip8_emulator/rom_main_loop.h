#ifndef CHIP_8_EMULATOR_ROM_MAIN_LOOP_H
#define CHIP_8_EMULATOR_ROM_MAIN_LOOP_H

#include <string_view>

namespace ch8
{
    class Chip8;
    class Window;

    // Execute the current ROM loaded in the chip8 emulator
    void executeCurrentROM(Chip8 &chip8, Window& window);
}

#endif //CHIP_8_EMULATOR_ROM_MAIN_LOOP_H
