#ifndef MOTOR_PREFAB_LOADER_H
#define MOTOR_PREFAB_LOADER_H

#include "json.h"
#include "motor/core/utility.h"
#include <entt/entity/registry.hpp>
#include <iostream>
#include <nameof.hpp>
#include <nlohmann/json.hpp>
#include <string_view>
#include <unordered_map>

namespace motor {

template<typename Entity>
class basic_prefab_loader {
    using entity_type = Entity;
    using registry_type = entt::basic_registry<entity_type>;

    struct entity_desc {
        entity_type entity;
        std::unordered_map<std::string_view, const nlohmann::json*> components;
    };

public:
    basic_prefab_loader(registry_type& destination) noexcept
        : reg{&destination} {}
    basic_prefab_loader(basic_prefab_loader&&) = default;

    basic_prefab_loader& operator=(basic_prefab_loader&&) = default;

    basic_prefab_loader& entities(const nlohmann::json& json) {
        for (auto& [ekey, evalue] : json.items()) {
            entity_desc desc;
            for (auto& [ckey, cvalue] : evalue.items()) {
                desc.entity = reg->create();
                desc.components.insert_or_assign(ckey, &cvalue);
            }
            descs.insert_or_assign(ekey, std::move(desc));
        }
        return *this;
    }

    template<typename Component>
    const basic_prefab_loader& component() const {
        auto component_name = motor::nameof_type<Component>();
        for (auto& [name, desc] : descs) {
            if (auto& it = desc.components.find(component_name);
                it != desc.components.end()) {
                Component instance{};
                serialize(json_input_archive{*it->second}, instance);
                reg->template emplace<Component>(desc.entity, instance);
            }
        }
        return *this;
    }

private:
    registry_type* reg{};
    std::unordered_map<std::string_view, entity_desc> descs{};
};

using prefab_loader = basic_prefab_loader<entt::entity>;

} // namespace motor

#endif // MOTOR_PREFAB_LOADER_H
