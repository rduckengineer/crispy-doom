#ifndef CRISPY_DOOM_ON_EXIT_HPP
#define CRISPY_DOOM_ON_EXIT_HPP

#include <utility>

template <typename Callable> struct onExit
{
    Callable fn;
    onExit(Callable&& fn_)
        : fn(std::move(fn_))
    {
    }
    ~onExit() { fn(); }
};

#endif // CRISPY_DOOM_ON_EXIT_HPP
