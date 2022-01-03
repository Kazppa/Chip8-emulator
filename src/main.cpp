#include "chip8_emulator/Chip8Gui.h"

#include <iostream>
#include <SDL.h>


int main(int argc, char *argv[])
{
    // Parsing arguments
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << "<ROM file path> "
                                             "<Screen resolution scale (optional Default=10)> "
                                             "<Frame Rate (optional Default=60)>" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string_view romFilePath(argv[1]);
    const auto videoScale = argc > 2 ? std::stoi(argv[2]) : ch8::Chip8Gui::DefaultScaleRatio;
    const auto frameRate = argc > 3 ? std::stoi(argv[3]) : ch8::Chip8Gui::DefaultFrameRate;

    // Initialize SDL 2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to initialize SDL : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Load and execute the ROM into the emulator
    ch8::Chip8Gui emulator(videoScale, frameRate);
    emulator.runROM(romFilePath);

    SDL_Quit();
    return EXIT_SUCCESS;
}
