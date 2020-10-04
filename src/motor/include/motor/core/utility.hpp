#ifndef MOTOR_UTILITY_HPP
#define MOTOR_UTILITY_HPP

#include "motor/core/type_traits.hpp"
#include <nameof.hpp>

namespace nameof {

template<typename T>
[[nodiscard]] constexpr std::string_view nameof_short_type() noexcept {
    static_assert(
            detail::nameof_type_supported<T>::value,
            "nameof::nameof_type unsupported compiler "
            "(https://github.com/Neargye/nameof#compiler-compatibility).");
    using U = detail::identity<detail::remove_cvref_t<T>>;
    constexpr std::string_view name =
            detail::pretty_name(detail::type_name_v<U>);
    static_assert(name.size() > 0, "Type does not have a name.");

    return name;
}

} // namespace nameof

#endif // MOTOR_UTILITY_HPP
