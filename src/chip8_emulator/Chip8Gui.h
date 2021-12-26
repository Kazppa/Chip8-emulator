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

        // Main loop, Chip8 frame rate is 60Hz
        void runROM(std::string_view romFilePath, int frameRate = 60);

    private:
        void render();

        void processEvent(const SDL_Event &sdlEvent);

        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        int _pitch;
        Chip8 _chip8;
        bool _isRunning = false;
    };
}

#endif //CHIP_8_EMULATOR_CHIP8GUI_H
