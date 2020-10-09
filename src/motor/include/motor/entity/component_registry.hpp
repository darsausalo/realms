#ifndef MOTOR_COMPONENTS_HPP
#define MOTOR_COMPONENTS_HPP

#include "motor/core/lua_archive.hpp"
#include "motor/core/utility.hpp"
#include <entt/entity/registry.hpp>
#include <unordered_map>

namespace motor {

enum class component_specifier : std::uint8_t { FINAL, OVERRIDABLE };

class component_registry {
    using transpire_fn_type = void(entt::registry& reg, entt::entity e,
                                   lua_input_archive& ar);
    using stamp_fn_type = void(const entt::registry&, const entt::entity,
                               entt::registry&, const entt::entity);
    using clone_fn_type = void(const entt::registry&, entt::registry&);

    template<typename Component>
    static void transpire(entt::registry& reg, entt::entity e,
                          lua_input_archive& ar) {
        if constexpr (std::is_empty_v<Component>) {
            reg.template emplace_or_replace<Component>(e);
        } else {
            Component instance{};
            serialize(ar, instance);
            reg.template emplace_or_replace<Component>(e,
                                                       std::as_const(instance));
        }
    }

    template<typename Component>
    static void stamp(const entt::registry& from, const entt::entity src,
                      entt::registry& to, const entt::entity dst) {
        if constexpr (std::is_empty_v<Component>) {
            to.template emplace_or_replace<Component>(dst);
        } else {
            to.emplace_or_replace<Component>(dst, from.get<Component>(src));
        }
    }

    template<typename Component>
    static void patch(const entt::registry& from, const entt::entity src,
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
    static void clone(const entt::registry& from, entt::registry& to) {
        const auto* data = from.data<Component>();
        const auto size = from.size<Component>();

        if constexpr (std::is_empty_v<Component>) {
            to.insert<Component>(data, data + size);
        } else {
            const auto* raw = from.raw<Component>();
            to.insert<Component>(data, data + size, raw, raw + size);
        }
    }

public:
    template<typename Component,
             component_specifier Specifier = component_specifier::OVERRIDABLE>
    void define() noexcept {
        constexpr auto name = nameof::nameof_short_type<Component>();
        auto name_id = entt::hashed_string::value(std::data(name));
        constexpr auto type_id = entt::type_hash<Component>::value();
        transpire_functions[name_id] =
                &component_registry::transpire<Component>;
        stamp_functions[type_id] = &component_registry::stamp<Component>;
        clone_functions[type_id] = &component_registry::clone<Component>;
        patch_functions[type_id] = &component_registry::patch<Component>;
        specifiers[type_id] = Specifier;
    }

    template<entt::id_type Value,
             component_specifier Specifier = component_specifier::OVERRIDABLE>
    void define() noexcept {
        constexpr auto name_id = Value;
        constexpr auto type_id = entt::type_hash<entt::tag<Value>>::value();
        transpire_functions[name_id] =
                &component_registry::transpire<entt::tag<Value>>;
        stamp_functions[type_id] = &component_registry::stamp<entt::tag<Value>>;
        clone_functions[type_id] = &component_registry::clone<entt::tag<Value>>;
        patch_functions[type_id] = &component_registry::patch<entt::tag<Value>>;
        specifiers[type_id] = Specifier;
    }

    bool is_defined(entt::id_type name_id) noexcept {
        return transpire_functions.find(name_id) != transpire_functions.cend();
    }

    void transpire(entt::registry& reg, entt::entity e, lua_input_archive& ar,
                   entt::id_type name_id) noexcept {
        transpire_functions[name_id](reg, e, ar);
    }

    void stamp(const entt::registry& from, const entt::entity src,
               entt::registry& to, const entt::entity dst) noexcept {
        from.visit(src, [this, &from, &to, src, dst](const auto type) {
            stamp_functions[type.hash()](from, src, to, dst);
        });
    }

    void patch(const entt::registry& from, const entt::entity src,
               entt::registry& to, const entt::entity dst) noexcept {
        from.visit(src, [this, &from, &to, src, dst](const auto type) {
            if (specifiers[type.hash()] == component_specifier::OVERRIDABLE) {
                patch_functions[type.hash()](from, src, to, dst);
            }
        });
    }

private:
    std::unordered_map<entt::id_type, transpire_fn_type*> transpire_functions{};
    std::unordered_map<entt::id_type, stamp_fn_type*> stamp_functions{};
    std::unordered_map<entt::id_type, clone_fn_type*> clone_functions{};
    std::unordered_map<entt::id_type, stamp_fn_type*> patch_functions{};
    std::unordered_map<entt::id_type, component_specifier> specifiers{};
};

struct prototype {
    entt::entity value;
};

template<typename Archive>
void serialize(Archive& ar, prototype& value) {
    ar.member(M(value.value));
}

} // namespace motor

#endif // MOTOR_COMPONENTS_HPP
