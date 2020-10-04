#ifndef MOTOR_PROTOTYPE_REGISTRY_HPP
#define MOTOR_PROTOTYPE_REGISTRY_HPP

#include "motor/core/lua_archive.hpp"
#include <entt/entity/registry.hpp>
#include <sol/forward.hpp>
#include <unordered_map>
#include <vector>

namespace motor {

class prototype_registry {
public:
    prototype_registry() noexcept = default;
    ~prototype_registry() noexcept = default;

    void transpire(const sol::table& defs);

    entt::entity spawn(entt::registry& to, entt::id_type name_id);
    entt::entity get(entt::id_type name_id) const;

    void respawn(entt::registry& to);

private:
    std::unordered_map<entt::id_type, entt::entity> prototypes{};
    entt::registry reg{};

    entt::entity get_or_create(entt::id_type name_id);
};

} // namespace motor

#endif // MOTOR_PROTOTYPE_REGISTRY_HPP
