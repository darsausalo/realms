#ifndef MOTOR_PLUGIN_CONTEXT_H
#define MOTOR_PLUGIN_CONTEXT_H

#include "motor/core/binary_archive.h"
#include "motor/core/prefab_loader.h"
#include "motor/core/system_dispatcher.h"
#include "motor/core/system_module.h"
#include <entt/entity/fwd.hpp>
#include <string_view>

namespace motor {

class plugin_context : public system_module_context {
public:
    plugin_context() = default;
    ~plugin_context() = default;

    void add_systems(system_dispatcher& dispatcher);
    void remove_systems(system_dispatcher& dispatcher);

    void load_prefabs(prefab_loader& loader);
    void load_snapshot(const entt::snapshot_loader& ss_loadr,
                       binary_input_archive& ar);
    void save_snapshot(const entt::snapshot& ss, binary_output_archive& ar);

    std::shared_ptr<spdlog::logger> get_logger() const override;

protected:
    void add_module(std::string_view module_name,
                    std::unique_ptr<system_module>&& module_instance) override;

private:
    struct module_desc {
        std::string name;
        std::unique_ptr<system_module> instance;

        module_desc() = default;
        module_desc(std::string_view name,
                    std::unique_ptr<system_module>&& instance)
            : name{name}, instance{std::move(instance)} {}
    };

    std::vector<module_desc> modules;
};

} // namespace motor

#endif // MOTOR_PLUGIN_CONTEXT_H
