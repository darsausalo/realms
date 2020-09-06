#ifndef MOTOR_TYPE_TRAITS_H
#define MOTOR_TYPE_TRAITS_H

#include <array>
#include <type_traits>

namespace motor {

template<typename T>
struct is_array : std::false_type {};

template<typename T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template<typename T>
struct type_name : std::false_type {
    static constexpr char* name = nullptr;
};

} // namespace motor

#endif // MOTOR_TYPE_TRAITS_H