#include "chip8_emulator/Chip8.h"
#include "chip8_emulator/Window.hpp"

#include <iostream>
#include <SDL.h>
#include <chrono>

// Execute the current ROM loaded in the chip8 emulator
void mainLoop(ch8::Chip8 &chip8, ch8::Window &window)
{
    // const float frameDelay = static_cast<float>(window._frameRate) / 60.0f;
     const float frameDelay = 1.0f;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    while (!quit) {
        window.processInput(chip8._keypad, quit);

        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float timeDelta = std::chrono::duration<float,
                std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (timeDelta <= frameDelay) {
            continue;
        }

        // next CPU cycle
        lastCycleTime = currentTime;

        // Emulate 1 CPU cycle
        chip8.execCpuCycle();

        if (chip8.renderRequired()) {
            chip8.setRenderRequired(false);
            window.render(chip8._video.data());
        }
    }
}


int main(int argc, char *argv[])
{
    // Initialize SDL 2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to initialize SDL : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Parsing arguments
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <ROM file path>"
                                             " <Screen resolution scale (Optional Default=10)>"
                                             " <Frame Rate (Optional Default=60)>" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string_view romFilePath(argv[1]);
    const auto videoScale = argc > 2 ? std::stoi(argv[2]) : ch8::Window::DefaultScaleRatio;
    const auto frameRate = argc > 3 ? std::stoi(argv[3]) : ch8::Window::DefaultFrameRate;

    // Load and execute the ROM into the emulator
    ch8::Chip8 chip8Emulator;
    if (!chip8Emulator.loadROM(romFilePath)) {
        SDL_Log("Failed to load the given ROM at location \"%s\"", romFilePath.data());
        return EXIT_FAILURE;
    }
    ch8::Window window(videoScale, frameRate);
    mainLoop(chip8Emulator, window);

    SDL_Quit();
    return EXIT_SUCCESS;
}
