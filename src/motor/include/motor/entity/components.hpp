#ifndef MOTOR_COMPONENTS_HPP
#define MOTOR_COMPONENTS_HPP

#include "motor/core/lua_archive.hpp"
#include "motor/core/utility.hpp"
#include <entt/entity/registry.hpp>
#include <unordered_map>

namespace motor {

enum class component_specifier : std::uint8_t { FINAL, OVERRIDABLE };

namespace internal {

struct component_context {
    using transpire_fn_type = void(entt::registry& reg, entt::entity e,
                                   lua_input_archive& ar);
    using stamp_fn_type = void(const entt::registry&, const entt::entity,
                               entt::registry&, const entt::entity);
    using clone_fn_type = void(const entt::registry&, entt::registry&);

    inline static std::unordered_map<entt::id_type, transpire_fn_type*>
            transpire_functions;
    inline static std::unordered_map<entt::id_type, stamp_fn_type*>
            stamp_functions;
    inline static std::unordered_map<entt::id_type, clone_fn_type*>
            clone_functions;
    inline static std::unordered_map<entt::id_type, stamp_fn_type*>
            patch_functions;
    inline static std::unordered_map<entt::id_type, component_specifier>
            specifiers;
};

} // namespace internal

namespace components {

template<typename Component>
void transpire(entt::registry& reg, entt::entity e, lua_input_archive& ar) {
    if constexpr (std::is_empty_v<Component>) {
        reg.template emplace_or_replace<Component>(e);
    } else {
        Component instance{};
        serialize(ar, instance);
        reg.template emplace_or_replace<Component>(e, std::as_const(instance));
    }
}

template<typename Component>
void stamp(const entt::registry& from, const entt::entity src,
           entt::registry& to, const entt::entity dst) {
    if constexpr (std::is_empty_v<Component>) {
        to.template emplace_or_replace<Component>(dst);
    } else {
        to.emplace_or_replace<Component>(dst, from.get<Component>(src));
    }
}

template<typename Component>
void patch(const entt::registry& from, const entt::entity src,
           entt::registry& to, const entt::entity dst) {
    if constexpr (std::is_empty_v<Component>) {
        if (to.template has<Component>(dst)) {
            to.template emplace_or_replace<Component>(dst);
        }
    } else {
        if (to.template has<Component>(dst)) {
            to.emplace_or_replace<Component>(dst, from.get<Component>(src));
        }
    }
}

template<typename Component>
void clone(const entt::registry& from, entt::registry& to) {
    const auto* data = from.data<Component>();
    const auto size = from.size<Component>();

    if constexpr (std::is_empty_v<Component>) {
        to.insert<Component>(data, data + size);
    } else {
        const auto* raw = from.raw<Component>();
        to.insert<Component>(data, data + size, raw, raw + size);
    }
}

template<typename Component,
         component_specifier Specifier = component_specifier::OVERRIDABLE>
void define() noexcept {
    constexpr auto name = nameof::nameof_short_type<Component>();
    auto name_id = entt::hashed_string::value(std::data(name));
    constexpr auto type_id = entt::type_info<Component>::id();
    internal::component_context::transpire_functions[name_id] =
            &transpire<Component>;
    internal::component_context::stamp_functions[type_id] = &stamp<Component>;
    internal::component_context::clone_functions[type_id] = &clone<Component>;
    internal::component_context::patch_functions[type_id] = &patch<Component>;
    internal::component_context::specifiers[type_id] = Specifier;
}

template<entt::id_type Value,
         component_specifier Specifier = component_specifier::OVERRIDABLE>
void define() noexcept {
    constexpr auto name_id = Value;
    constexpr auto type_id = entt::type_info<entt::tag<Value>>::id();
    internal::component_context::transpire_functions[name_id] =
            &transpire<entt::tag<Value>>;
    internal::component_context::stamp_functions[type_id] =
            &stamp<entt::tag<Value>>;
    internal::component_context::clone_functions[type_id] =
            &clone<entt::tag<Value>>;
    internal::component_context::patch_functions[type_id] =
            &patch<entt::tag<Value>>;
    internal::component_context::specifiers[type_id] = Specifier;
}

inline bool is_defined(entt::id_type name_id) noexcept {
    using namespace internal;
    return component_context::transpire_functions.find(name_id) !=
           component_context::transpire_functions.cend();
}

inline void transpire(entt::registry& reg, entt::entity e,
                      lua_input_archive& ar, entt::id_type name_id) noexcept {
    using namespace internal;
    component_context::transpire_functions[name_id](reg, e, ar);
}

inline void stamp(const entt::registry& from, const entt::entity src,
                  entt::registry& to, const entt::entity dst) noexcept {
    using namespace internal;
    from.visit(src, [&from, &to, src, dst](const auto type_id) {
        component_context::stamp_functions[type_id](from, src, to, dst);
    });
}

inline void patch(const entt::registry& from, const entt::entity src,
                  entt::registry& to, const entt::entity dst) noexcept {
    using namespace internal;
    from.visit(src, [&from, &to, src, dst](const auto type_id) {
        if (component_context::specifiers[type_id] ==
            component_specifier::OVERRIDABLE) {
            component_context::patch_functions[type_id](from, src, to, dst);
        }
    });
}

} // namespace components

struct prototype {
    entt::entity value;
};

template<typename Archive>
void serialize(Archive& ar, prototype& value) {
    ar.member(M(value.value));
}

} // namespace motor

#endif // MOTOR_COMPONENTS_HPP
