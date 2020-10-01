#ifndef MOTOR_COMPONENTS_SERVICE_H
#define MOTOR_COMPONENTS_SERVICE_H

#include "motor/core/lua_archive.h"
#include "motor/core/utility.h"
#include <entt/entity/registry.hpp>
#include <unordered_map>

namespace motor {

class components_service {
    template<typename Component>
    static void prepare(entt::registry& reg) {
        reg.prepare<Component>();
    }

    template<typename Component>
    static void transpire(entt::registry& reg, entt::entity e,
                          lua_input_archive& ar) {
        if constexpr (std::is_empty_v<Component>) {
            reg.template emplace<Component>(e);
        } else {
            Component instance{};
            serialize(ar, instance);
            reg.template emplace<Component>(e, std::as_const(instance));
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
    void define_component() {
        constexpr auto name = nameof::nameof_short_type<Component>();
        auto name_id = entt::hashed_string::value(std::data(name));
        constexpr auto type_id = entt::type_info<Component>::id();
        transpire_functions[name_id] =
                &components_service::transpire<Component>;
        stamp_functions[type_id] = &components_service::stamp<Component>;

        prepare_functions.push_back(&components_service::prepare<Component>);
    }

    template<entt::id_type Value>
    void define_tag() {
        constexpr auto name_id = Value;
        constexpr auto type_id = entt::type_info<entt::tag<Value>>::id();
        transpire_functions[name_id] =
                &components_service::transpire<entt::tag<Value>>;
        stamp_functions[type_id] = &components_service::stamp<entt::tag<Value>>;

        prepare_functions.push_back(
                &components_service::prepare<entt::tag<Value>>);
    }

public:
    components_service() noexcept = default;
    components_service(const components_service&) = delete;
    components_service(components_service&&) = delete;

    components_service& operator=(const components_service&) = delete;
    components_service& operator=(components_service&&) = delete;

    ~components_service() noexcept = default;

    template<typename... Component>
    void component() noexcept {
        (define_component<Component>(), ...);
    }

    template<entt::id_type... Value>
    void tag() noexcept {
        (define_tag<Value>(), ...);
    }

    bool exists(entt::id_type name_id) noexcept {
        return transpire_functions.find(name_id) != transpire_functions.cend();
    }

    void prepare(entt::registry& reg) {
        for (auto&& p : prepare_functions) {
            p(reg);
        }
    }

    void transpire(entt::registry& reg, entt::entity e, lua_input_archive& ar,
                   entt::id_type name_id) noexcept {
        transpire_functions[name_id](reg, e, ar);
    }

    void stamp(const entt::registry& from, const entt::entity src,
               entt::registry& to, const entt::entity dst) noexcept {
        from.visit(src, [this, &from, &to, src, dst](const auto type_id) {
            stamp_functions[type_id](from, src, to, dst);
        });
    }

private:
    using prepare_fn = void(entt::registry& reg);
    using transpire_fn = void(entt::registry& reg, entt::entity e,
                              lua_input_archive& ar);
    using stamp_fn_type = void(const entt::registry&, const entt::entity,
                               entt::registry&, const entt::entity);

    std::vector<prepare_fn*> prepare_functions;
    std::unordered_map<entt::id_type, transpire_fn*> transpire_functions;
    std::unordered_map<entt::id_type, stamp_fn_type*> stamp_functions;
};

} // namespace motor

#endif // MOTOR_COMPONENTS_SERVICE_H
