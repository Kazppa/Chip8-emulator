#include "chip8_emulator/Chip8.h"
#include "chip8_emulator/Window.hpp"
#include "chip8_emulator/os_features.h"

#include <iostream>
#include <format>
#include <thread>
#include <SDL.h>


// Execute the current ROM loaded in the chip8 emulator
void executeROM(ch8::Chip8 &chip8, ch8::Window &window);


int main(int argc, char *argv[])
{
    // Parsing arguments
    int videoScale;
    if (argc > 3) {
        std::cerr << std::format("Usage: {} <Screen resolution upscale ratio (Optional Default={})>",
                                 argv[0], ch8::Window::DefaultScaleRatio);
        return EXIT_FAILURE;
    }
    try {
        videoScale = argc > 2 ? std::stoi(argv[1]) : ch8::Window::DefaultScaleRatio;
    }
    catch (...) {
        std::cerr << "invalid integer value for screen resolution upscale";
        return EXIT_FAILURE;
    }

    // Initialize SDL 2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to initialize SDL : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Ask user the path to the ROM file
    const auto romFilePath = ch8::os::getFilePathDialog();
    if (romFilePath.empty()) {
        // User closed file dialog
        return EXIT_SUCCESS;
    }

    // Load the ROM binary file in memory
    ch8::Chip8 chip8Emulator;
    if (!chip8Emulator.loadROM(romFilePath)) {
        return EXIT_FAILURE;
    }
    ch8::Window window(videoScale);
    // Main loop
    executeROM(chip8Emulator, window);

    SDL_Quit();
    return EXIT_SUCCESS;
}


void executeROM(ch8::Chip8 &chip8, ch8::Window &window)
{
    using namespace std::chrono_literals;

    bool quit = false;
    do {
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
    } while (!quit);
}
