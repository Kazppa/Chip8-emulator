#ifndef CHIP_8_EMULATOR_CHIP8GUI_H
#define CHIP_8_EMULATOR_CHIP8GUI_H

#include <SDL_events.h>

#include "Chip8.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace ch8
{
    class Chip8Gui
    {
    public:
        explicit Chip8Gui(int videoScale);

        ~Chip8Gui();

        // Main loop to execute a ROM, Chip8 official frame rate is 60Hz
        void runROM(std::string_view romFilePath, int frameRate = 60);

    private:
        void render();

        void processEvent(const SDL_Event &sdlEvent);

        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        Chip8 _chip8;
        bool _isRunning = false;

        // Length of a row of pixels (in bytes)
        static constexpr int VideoPitch = sizeof(decltype(Chip8::_video)::value_type) * Chip8::VIDEO_WIDTH;
    };
}

#endif //CHIP_8_EMULATOR_CHIP8GUI_H
