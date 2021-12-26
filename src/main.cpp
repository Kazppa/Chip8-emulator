#include "chip8_emulator//Chip8Gui.h"

#include <iostream>
#include <SDL.h>


int main(int argc, char *argv[])
{
    // Parse arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Frame Rate> <ROM file path>" << std::endl;
        return EXIT_FAILURE;
    }

    const auto videoScale = std::stoi(argv[1]);
    const auto frameRate = std::stoi(argv[2]);
    const std::string_view romFilePath = argv[3];

    // Initialize SDL 2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to initialize SDL : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Execute the ROM on the emulator
    ch8::Chip8Gui emulator(videoScale);
    emulator.runROM(romFilePath, frameRate);

    SDL_Quit();
    return EXIT_SUCCESS;
}
