#ifndef MOTOR_PROTOTYPE_REGISTRY_H
#define MOTOR_PROTOTYPE_REGISTRY_H

#include "motor/core/lua_archive.h"
#include <entt/entity/registry.hpp>
#include <sol/forward.hpp>
#include <unordered_map>
#include <vector>

namespace motor {

class prototype_registry {
public:
    explicit prototype_registry(const sol::table& defs);
    ~prototype_registry() = default;

    entt::entity spawn(entt::registry& to, entt::id_type name_id);
    entt::entity get(entt::id_type name_id) const;

private:
    std::unordered_map<entt::id_type, entt::entity> prototypes{};
    entt::registry reg{};

    entt::entity get_or_create(entt::id_type name_id);
};

} // namespace motor

#endif // MOTOR_PROTOTYPE_REGISTRY_H
