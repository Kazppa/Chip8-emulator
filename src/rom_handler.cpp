#include "chip8_emulator/rom_handler.h"

#include "chip8_emulator/Chip8.h"
#include "chip8_emulator/Window.hpp"

#include <SDL.h>

// Execute the current ROM loaded in the chip8 emulator
void ch8::runMainLoop(ch8::Chip8 &chip8, ch8::Window &window)
{
    // 60Hz frequency
    const unsigned int tickDelay = 1000 / 100;

    //unsigned int nextCycleTick = 0u;
    bool quit = false;
    while (!quit) {
        window.processInput(chip8._keypad, quit);

        // Emulate 1 CPU cycle
        chip8.execCpuCycle();

        if (chip8.renderRequired()) {
            chip8.setRenderRequired(false);
            window.render(chip8._video.data());

            SDL_Delay(tickDelay);
        }

        // const auto currentTick = SDL_GetTicks();
        //nextCycleTick = currentTick + tickDelay;
    }
}
