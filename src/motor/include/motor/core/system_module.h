#ifndef MOTOR_SYSTEM_MODULE_H
#define MOTOR_SYSTEM_MODULE_H

#include "motor/core/binary_archive.h"
#include "motor/core/prefab_loader.h"
#include "motor/core/system_dispatcher.h"
#include "motor/core/utility.h"
#include <entt/entity/fwd.hpp>
#include <memory>
#include <spdlog/fwd.h>
#include <type_traits>

namespace motor {

class system_module;

class system_module_context {
public:
    virtual ~system_module_context() = default;

    template<typename Module>
    void register_module() {
        static_assert(
                std::is_base_of_v<system_module, Module>,
                "System module should be derived from motor::system_module");
        add_module(nameof_type<Module>(), std::make_unique<Module>());
    }

    virtual std::shared_ptr<spdlog::logger> get_logger() const = 0;

protected:
    system_module_context() = default;

    virtual void
    add_module(std::string_view module_name,
               std::unique_ptr<system_module>&& module_instance) = 0;
};

class system_module {
public:
    system_module(system_module&&) = default;
    system_module& operator=(system_module&&) = default;
    virtual ~system_module() = default;

protected:
    system_module() = default;

    template<typename... Component>
    void component() {
        prefab_loaders.push_back(
                [](auto& loader) { loader.component<Component...>(); });
        snapshot_loaders.push_back([](auto& ss_loader, auto& ar) {
            ss_loader.component<Component...>(ar);
        });
        snapshot_savers.push_back(
                [](auto& ss, auto& ar) { ss.component<Component...>(ar); });
    }

    template<typename System, typename... Dependencies>
    void system() {
        system_adders.push_back(
                [](auto& d) { d.add_system<System, Dependencies...>(); });
        system_removers.push_back([](auto& d) { d.remove_system<System>(); });
    }

private:
    friend class plugin_context;

    using load_prefabs_func = std::function<void(prefab_loader&)>;
    using load_snapshot_func = std::function<void(const entt::snapshot_loader&,
                                                  binary_input_archive&)>;
    using save_snapshot_func =
            std::function<void(const entt::snapshot&, binary_output_archive&)>;

    using system_func = std::function<void(system_dispatcher&)>;

    std::vector<load_prefabs_func> prefab_loaders;
    std::vector<load_snapshot_func> snapshot_loaders;
    std::vector<save_snapshot_func> snapshot_savers;

    std::vector<system_func> system_adders;
    std::vector<system_func> system_removers;
};

} // namespace motor

#endif // MOTOR_SYSTEM_MODULE_H
