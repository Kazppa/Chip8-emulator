#include "chip8_emulator/Chip8Gui.h"

#include <chrono>
#include <stdexcept>

#include "SDL.h"


ch8::Chip8Gui::Chip8Gui(int videoScale, int frameRate) : _frameRate(frameRate)
{
    _window = SDL_CreateWindow("CHIP-8 Emulator", 0, 0,
                               Chip8::VIDEO_WIDTH * videoScale, Chip8::VIDEO_HEIGHT * videoScale, SDL_WINDOW_SHOWN);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                 Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);
}

ch8::Chip8Gui::~Chip8Gui()
{
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

void ch8::Chip8Gui::render()
{
    SDL_UpdateTexture(_texture, nullptr, _chip8._video.data(), VideoPitch);
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);
}

void ch8::Chip8Gui::processEvent(const SDL_Event &sdlEvent)
{
    // Map the current sdl input with the chip-8's associated key index
    constexpr auto sdlKeyMapper = [](SDL_Keycode sdlCode) -> Chip8::Key {
        switch (sdlCode) {
            case SDLK_x:
                return Chip8::Key_x;
            case SDLK_1:
                return Chip8::Key_1;
            case SDLK_2:
                return Chip8::Key_2;
            case SDLK_3:
                return Chip8::Key_3;
            case SDLK_q:
                return Chip8::Key_q;
            case SDLK_w:
                return Chip8::Key_w;
            case SDLK_e:
                return Chip8::Key_e;
            case SDLK_a:
                return Chip8::Key_a;
            case SDLK_s:
                return Chip8::Key_s;
            case SDLK_d:
                return Chip8::Key_d;
            case SDLK_z:
                return Chip8::Key_z;
            case SDLK_c:
                return Chip8::Key_c;
            case SDLK_4:
                return Chip8::Key_4;
            case SDLK_r:
                return Chip8::Key_r;
            case SDLK_f:
                return Chip8::Key_f;
            case SDLK_v:
                return Chip8::Key_v;

            default:
                return Chip8::Key_INVALID;
        }
    };

    switch (sdlEvent.type) {
        case SDL_QUIT:
            // Stop program
            _isRunning = false;
            break;

        case SDL_KEYDOWN: {
            const auto sdlKey = sdlEvent.key.keysym.sym;
            if (const auto keyIndex = sdlKeyMapper(sdlKey);
                    keyIndex != Chip8::Key_INVALID) {
                _chip8._keypad[keyIndex] = 1;
            }
            else if (sdlKey == SDLK_ESCAPE) {
                _isRunning = false;
            }
            break;
        }
        case SDL_KEYUP: {
            if (const auto keyIndex = sdlKeyMapper(sdlEvent.key.keysym.sym);
                    keyIndex != Chip8::Key_INVALID) {
                _chip8._keypad[keyIndex] = 0;
            }
            break;
        }
    }
}

void ch8::Chip8Gui::runROM(std::string_view romFilePath)
{
    using Clock = std::chrono::high_resolution_clock;

    if (_isRunning) {
        throw std::runtime_error("A ROM is already running !");
    }
    if (!_chip8.loadROM(romFilePath)) {
        std::string error_msg = "Failed to load ROM : ";
        error_msg.append(romFilePath);
        throw std::runtime_error(error_msg);
        return;
    }
    _isRunning = true;

    const float frameRatePerSec = static_cast<float>(_frameRate) / 60.0f;
    auto lastUpdateTime = Clock::now();
    SDL_Event sdlEvent;
    while (_isRunning) {
        while (SDL_WaitEvent(&sdlEvent) != 0) {
            if (!_isRunning) {
                return;
            }
            this->processEvent(sdlEvent);

            const auto currentTime = Clock::now();
            const auto deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(
                    currentTime - lastUpdateTime).count();

            if (deltaTime > frameRatePerSec) {
                lastUpdateTime = currentTime;
                _chip8.update(); // Execute 1 CPU cycle
                this->render();  // Render on screen
            }
        }
    }
}
