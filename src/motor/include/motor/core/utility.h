#ifndef MOTOR_UTILITY_H
#define MOTOR_UTILITY_H

#include "motor/core/type_traits.h"
#include <nameof.hpp>

namespace motor {

template<typename T>
inline constexpr std::string_view nameof_type() {
    if constexpr (type_name<T>::value) {
        return type_name<T>::name;
    }

    static constexpr char struct_prefix[] = "struct ";
    static constexpr std::size_t struct_prefix_size = sizeof(struct_prefix) - 1;
    static constexpr char class_prefix[] = "class ";
    static constexpr std::size_t class_prefix_size = sizeof(class_prefix) - 1;

    std::string_view type_name = nameof::nameof_type<T>();
    if (auto n = type_name.find(struct_prefix); n != std::string_view::npos) {
        type_name = type_name.substr(struct_prefix_size);
    } else if (auto n = type_name.find(class_prefix);
               n != std::string_view::npos) {
        type_name = type_name.substr(class_prefix_size);
    }

    return type_name;
}

} // namespace motor

#endif // MOTOR_UTILITY_H
