#include "chip8_emulator/Window.hpp"

#include "chip8_emulator/Chip8.h"

#include <SDL.h>

using ch8::Window;

namespace {
    // Length of a row of pixels (in bytes)
    constexpr int VideoPitch = sizeof(decltype(ch8::Chip8::_video)::value_type) *
            ch8::Chip8::VIDEO_WIDTH;
}

Window::Window(int videoScale, int frameRate) : _frameRate(frameRate)
{
    _window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               Chip8::VIDEO_WIDTH * videoScale, Chip8::VIDEO_HEIGHT * videoScale, SDL_WINDOW_SHOWN);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                 Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);
}

Window::~Window()
{
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}

void Window::render(std::uint32_t *pixels)
{
    SDL_UpdateTexture(_texture, nullptr, pixels, VideoPitch);
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);
}

void Window::processInput(std::array<uint8_t, 16> &keys, bool *quit)
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

    *quit = false;
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                // Stop program
                *quit = true;
                break;

            case SDL_KEYDOWN: {
                const auto sdlKey = sdlEvent.key.keysym.sym;
                if (const auto keyIndex = sdlKeyMapper(sdlKey);
                        keyIndex != Chip8::Key_INVALID) {
                        keys[keyIndex] = 1u;
                } else if (sdlKey == SDLK_ESCAPE) {
                    *quit = true;
                }
                break;
            }
            case SDL_KEYUP: {
                if (const auto keyIndex = sdlKeyMapper(sdlEvent.key.keysym.sym);
                        keyIndex != Chip8::Key_INVALID) {
                    keys[keyIndex] = 0;
                }
                break;
            }
        }
    }
}