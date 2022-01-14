#ifndef CHIP_8_EMULATOR_ROM_HANDLER_H
#define CHIP_8_EMULATOR_ROM_HANDLER_H

#include <string_view>

namespace ch8
{
    class Chip8;
    class Window;

    void runMainLoop(Chip8 &chip8, Window& window);
}

#endif //CHIP_8_EMULATOR_ROM_HANDLER_H
