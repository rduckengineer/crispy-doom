#ifndef CRISPY_DOOM_OVERLOAD_HPP
#define CRISPY_DOOM_OVERLOAD_HPP

#include <variant>

template <typename... Visitor> struct Overload : Visitor...
{
    Overload(Visitor&&... visitor)
        : Visitor{std::forward<Visitor>(visitor)}...
    {
    }
    using Visitor::operator()...;
};

template <typename R = void, typename... Visitor>
static auto checked_overload(Visitor&&... v)
{
    return Overload{
        [](std::monostate) -> R { throw std::bad_variant_access{}; },
        std::forward<Visitor>(v)...};
}

#endif // CRISPY_DOOM_OVERLOAD_HPP
