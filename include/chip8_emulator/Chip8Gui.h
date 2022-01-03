#ifndef CHIP_8_EMULATOR_CHIP8GUI_H
#define CHIP_8_EMULATOR_CHIP8GUI_H

#include "SDL_events.h"

#include "Chip8.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace ch8
{
    class Chip8Gui
    {
    public:
        // Launch an emulator with a scale ratio (for modern screen with high resolution)
        // Chip8 official frame rate is 60Hz
        explicit Chip8Gui(int videoScale = 10, int frameRate = 60);

        ~Chip8Gui();

        // Main loop to execute a ROM,
        void runROM(std::string_view romFilePath);

    private:
        void render();

        // Handle Key events
        void processEvent(const SDL_Event &sdlEvent);

        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        int _frameRate = 60;
        Chip8 _chip8;
        bool _isRunning = false;

        // Length of a row of pixels (in bytes)
        static constexpr int VideoPitch = sizeof(decltype(Chip8::_video)::value_type) * Chip8::VIDEO_WIDTH;
    };
}

#endif //CHIP_8_EMULATOR_CHIP8GUI_H
