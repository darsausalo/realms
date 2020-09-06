#ifndef MOTOR_UTILITY_H
#define MOTOR_UTILITY_H

#include <refl.hpp>

namespace motor {

template<typename T>
constexpr std::string_view nameof_type() {
    return refl::reflect<T>().name.c_str();
}

} // namespace motor

#endif // MOTOR_UTILITY_H
