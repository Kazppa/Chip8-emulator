#include "Chip8Gui.h"

#include <chrono>
#include <stdexcept>

#include <SDL.h>

namespace
{
    constexpr auto VIDEO_WIDTH = ch8::Chip8::VIDEO_WIDTH;
    constexpr auto VIDEO_HEIGHT = ch8::Chip8::VIDEO_HEIGHT;
}

ch8::Chip8Gui::Chip8Gui(int videoScale)
{
    _window = SDL_CreateWindow("CHIP-8 Emulator", 0, 0,
                               VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, SDL_WINDOW_SHOWN);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                 VIDEO_WIDTH, VIDEO_HEIGHT);
}

ch8::Chip8Gui::~Chip8Gui()
{
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

void ch8::Chip8Gui::render()
{
    SDL_UpdateTexture(_texture, nullptr, _chip8._video.data(), _pitch);
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);
}


void ch8::Chip8Gui::processEvent(const SDL_Event &sdlEvent)
{
    _isRunning = false;
}

void ch8::Chip8Gui::runROM(std::string_view romFilePath, int frameRate)
{
    using Clock = std::chrono::high_resolution_clock;

    if (_isRunning) {
        throw std::runtime_error("A ROM is already running !");
    }
    _isRunning = true;
    _chip8.loadROM(romFilePath);

    const float frameRatePerSec = static_cast<float>(frameRate) / 60.0f;
    auto lastUpdateTime = Clock::now();
    SDL_Event sdlEvent;
    while (_isRunning) {
        while (SDL_WaitEvent(&sdlEvent) != 0) {
            this->processEvent(sdlEvent);

            const auto currentTime = Clock::now();
            auto deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(
                    currentTime - lastUpdateTime).count();

            if (deltaTime > frameRatePerSec) {
                lastUpdateTime = currentTime;
                _chip8.update(); // Execute 1 CPU cycle
                this->render();  // Render on screen
            }
        }
    }
}
