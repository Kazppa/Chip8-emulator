#include "chip8_emulator/Chip8.h"
#include "chip8_emulator/Window.hpp"
#include "chip8_emulator/rom_handler.h"

#include <iostream>
#include <format>
#include <SDL.h>


// Return -1 if invalid value
int parsePositiveInteger(std::string_view arg)
{
    int output;
    const auto result = std::from_chars(arg.data(), arg.data() + arg.size(), output);

    if (result.ec != std::errc{}) {
        // Conversion failed
        output = -1;

        if (result.ec == std::errc::invalid_argument) {
            std::cerr << std::format("non integer argument \"{}\"", arg);
        }
        else if (result.ec == std::errc::result_out_of_range) {
            std::cerr << std::format("value out of range for argument \"{}\"", arg);
        }
        else {
            // Any other errors
            std::cerr << std::format("invalid value for argument \"{}\"", arg);
        }
    }
    else {
        if (output < 0) {
            output = -1;
            std::cerr << std::format("invalid negative value \"{}\"", arg);
        }
    }
    return output;
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
        std::cerr << std::format("Usage: {} <ROM file path>"
                                 " <Screen resolution scale (Optional Default={})>"
                                 " <Frame Rate (Optional Default={})>",
                                 argv[0], ch8::Window::DefaultScaleRatio, ch8::Window::DefaultFrameRate);
        return EXIT_FAILURE;
    }
    const std::string_view romFilePath(argv[1]);
    const auto videoScale = argc > 2 ? parsePositiveInteger(argv[2]) : ch8::Window::DefaultScaleRatio;
    const auto frameRate = argc > 3 ? parsePositiveInteger(argv[3]) : ch8::Window::DefaultFrameRate;
    if (videoScale == -1 || frameRate == -1) {
        return EXIT_FAILURE;
    }

    // Load and execute the ROM into the emulator
    ch8::Chip8 chip8Emulator;
    if (!chip8Emulator.loadROM(romFilePath)) {
        return EXIT_FAILURE;
    }
    ch8::Window window(videoScale, frameRate);
    ch8::runMainLoop(chip8Emulator, window);

    SDL_Quit();
    return EXIT_SUCCESS;
}
