#ifndef MOTOR_UTILITY_HPP
#define MOTOR_UTILITY_HPP

#include "motor/core/type_traits.hpp"

namespace motor {

template<typename Type>
constexpr void hash_combine(Type& seed, Type value) noexcept {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename Type>
class reverse {
    Type& iterable;

public:
    explicit reverse(Type& iterable)
        : iterable{iterable} {}
    auto begin() const { return std::rbegin(iterable); }
    auto end() const { return std::rend(iterable); }
};

} // namespace motor

#endif // MOTOR_UTILITY_HPP
