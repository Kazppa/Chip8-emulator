#ifndef CHIP_8_EMULATOR_WINDOW_HPP
#define CHIP_8_EMULATOR_WINDOW_HPP

#include <array>
#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace ch8
{
    class Window
    {
    public:
        static constexpr auto DefaultScaleRatio = 10;
        static constexpr auto DefaultFrameRate = 60;   // Chip8 official frame rate is 60Hz

        explicit Window(int videoScale = DefaultScaleRatio, int frameRate = DefaultFrameRate);

        ~Window();

        void render(std::uint32_t *pixels);

        void show();

        void hide();

        void processInput(std::array<uint8_t, 16>& keys, bool *quit);

        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        int _frameRate;
    };
}

#endif //CHIP_8_EMULATOR_WINDOW_HPP
