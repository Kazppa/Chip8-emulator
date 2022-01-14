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
        static constexpr auto DefaultScaleRatio = 20;
        static constexpr auto DefaultFrequency = 500; // in Hertz

        explicit Window(int videoScale = DefaultScaleRatio, int frequency = DefaultFrequency);

        ~Window();

        void render(std::uint32_t *pixels);

        void processInput(std::array<uint8_t, 16> &keys, bool &quit);

        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;
        int _frequency;
    };
}

#endif //CHIP_8_EMULATOR_WINDOW_HPP
