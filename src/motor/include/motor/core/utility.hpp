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

namespace motor {

template<typename Type>
constexpr void hash_combine(Type& seed, Type value) noexcept {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename Type>
class reverse {
    Type& iterable;

public:
    explicit reverse(Type& iterable) : iterable{iterable} {}
    auto begin() const { return std::rbegin(iterable); }
    auto end() const { return std::rend(iterable); }
};

} // namespace motor

#endif // MOTOR_UTILITY_HPP
