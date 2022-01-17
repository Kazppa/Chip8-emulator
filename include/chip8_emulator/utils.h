#ifndef CHIP_8_EMULATOR_UTILS_H
#define CHIP_8_EMULATOR_UTILS_H

#include <array>

namespace ch8::utils
{
    template<typename T, typename ...Ts, size_t array_size = sizeof...(Ts)>
    constexpr std::array<T, array_size> make_array(Ts ...ts)
    {
        return { (T) ts... };
    }

    // RAII Callback
    template<typename Callable>
    class ScopeCallback
    {
    public:
        explicit ScopeCallback(Callable callback) : _callback(std::move(callback)) {}

        ScopeCallback(const ScopeCallback<Callable> &other) = delete;

        ~ScopeCallback() { std::invoke(_callback); }

        Callable _callback;
    };

    // Execute the Callable object when exiting the current scope
    template<typename Callable>
    ScopeCallback<Callable> make_scope_callback(Callable&& callback);
}

template<typename Callable>
ch8::utils::ScopeCallback<Callable> ch8::utils::make_scope_callback(Callable &&callback)
{
    return ScopeCallback<Callable>(Callable(std::forward<Callable>(callback)));
}

#endif //CHIP_8_EMULATOR_UTILS_H
