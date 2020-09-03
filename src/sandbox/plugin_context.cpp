#include "plugin_context.h"

namespace motor {
void plugin_context::add_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_adders) {
            f(dispatcher);
        }
    }
}

void plugin_context::remove_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_removers) {
            f(dispatcher);
        }
    }
}

void plugin_context::load_prefabs(prefab_loader& loader) {
    for (auto& m : modules) {
        for (auto& f : m.instance->prefab_loaders) {
            f(loader);
        }
    }
}

void plugin_context::load_snapshot(entt::snapshot_loader& ss_loadr,
                                   binary_input_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_loaders) {
            f(ss_loadr, ar);
        }
    }
}

void plugin_context::save_shapshot(entt::snapshot& ss,
                                   binary_output_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_savers) {
            f(ss, ar);
        }
    }
}

void plugin_context::add_module(
        std::string_view module_name,
        std::unique_ptr<system_module>&& module_instance) {
    modules.emplace_back(module_name, std::move(module_instance));
}

} // namespace motor
