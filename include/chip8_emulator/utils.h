#ifndef CHIP_8_EMULATOR_UTILS_H
#define CHIP_8_EMULATOR_UTILS_H

#include <array>

namespace ch8
{
    template<typename T, typename ...Ts, size_t array_size = sizeof...(Ts)>
    constexpr std::array<T, array_size> make_array(Ts ...ts)
    {
        return { (T) ts... };
    }
}

#endif //CHIP_8_EMULATOR_UTILS_H
