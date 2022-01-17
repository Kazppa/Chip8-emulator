#include "chip8_emulator/rom_main_loop.h"

#include "chip8_emulator/Chip8.h"
#include "chip8_emulator/Window.hpp"

#include <SDL.h>
#include <thread>

// Execute the current ROM loaded in the chip8 emulator
void ch8::executeCurrentROM(ch8::Chip8 &chip8, ch8::Window &window)
{
    using namespace std::chrono_literals;

    bool quit = false;
    while (!quit) {
        // Emulate a single CPU cycle
        chip8.execCpuCycle();

        // Get Keyboard inputs, quit is true when the escape key is pressed
        window.processInput(chip8._keypad, quit);

        if (chip8.renderRequired()) {
            // Render a new image
            window.render(chip8._video.data());
            chip8.setRenderRequired(false);
        }

        // Emulate a constant CPU frequency (~ 500 Hertz)
        std::this_thread::sleep_for(1500us);
    }
}
