#ifndef MOTOR_COMPONENTS_HPP
#define MOTOR_COMPONENTS_HPP

#include "motor/core/lua_archive.h"
#include "motor/core/utility.h"
#include <entt/entity/registry.hpp>
#include <unordered_map>

namespace motor {

namespace components {

namespace internal {

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

struct component_context {
    using transpire_fn = void(entt::registry& reg, entt::entity e,
                              lua_input_archive& ar);
    using stamp_fn_type = void(const entt::registry&, const entt::entity,
                               entt::registry&, const entt::entity);

    inline static std::unordered_map<entt::id_type, transpire_fn*>
            transpire_functions;
    inline static std::unordered_map<entt::id_type, stamp_fn_type*>
            stamp_functions;
    inline static std::unordered_map<entt::id_type, stamp_fn_type*>
            patch_functions;
};

} // namespace internal

template<typename Component>
void define() noexcept {
    constexpr auto name = nameof::nameof_short_type<Component>();
    auto name_id = entt::hashed_string::value(std::data(name));
    constexpr auto type_id = entt::type_info<Component>::id();
    internal::component_context::transpire_functions[name_id] =
            &internal::transpire<Component>;
    internal::component_context::stamp_functions[type_id] =
            &internal::stamp<Component>;
    internal::component_context::patch_functions[type_id] =
            &internal::patch<Component>;
}

template<entt::id_type Value>
void define() noexcept {
    constexpr auto name_id = Value;
    constexpr auto type_id = entt::type_info<entt::tag<Value>>::id();
    internal::component_context::transpire_functions[name_id] =
            &internal::transpire<entt::tag<Value>>;
    internal::component_context::stamp_functions[type_id] =
            &internal::stamp<entt::tag<Value>>;
    internal::component_context::patch_functions[type_id] =
            &internal::patch<entt::tag<Value>>;
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
        component_context::patch_functions[type_id](from, src, to, dst);
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
