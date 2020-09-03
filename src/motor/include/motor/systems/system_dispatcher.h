#ifndef MOTOR_SYSTEM_DISPATCHER_H
#define MOTOR_SYSTEM_DISPATCHER_H

#include "motor/core/utility.h"
#include "motor/systems/system.h"
#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <iostream>

namespace motor {

class system_dispatcher final {
public:
    system_dispatcher(entt::registry& reg) noexcept : reg(reg) {}
    system_dispatcher(system_dispatcher&) = delete;
    system_dispatcher(system_dispatcher&&) = delete;

    system_dispatcher operator=(system_dispatcher&) = delete;
    system_dispatcher operator=(system_dispatcher&&) = delete;

    ~system_dispatcher();

    template<typename System, typename... Dependencies>
    System& add_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        auto it = std::find_if(systems.cbegin(), systems.cend(), [](auto&& sd) {
            return sd.type_id == entt::type_info<System>::id();
        });
        systems.push_back(system_desc{entt::type_info<System>::id(),
                                      nameof_type<System>(),
                                      {entt::type_info<Dependencies>::id()...},
                                      std::make_unique<System>()});
        systems.back().instance->on_start(reg);
        auto system = static_cast<System*>(systems.back().instance.get());
        sort();
        return *system;
    }

    template<typename System>
    void remove_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        auto it = std::find_if(systems.cbegin(), systems.cend(), [](auto&& sd) {
            return sd.type_id == entt::type_info<System>::id();
        });
        if (it != systems.end()) {
            it->instance->on_stop(reg);
            systems.erase(it);
            sort();
        }
    }

    void update();

    void dump() {
        for (auto&& sd : systems) {
            std::cout << sd.name << "[";
            for (auto&& id : sd.dependencies) {
                auto it = std::find_if(
                        systems.cbegin(), systems.cend(),
                        [&id](auto&& s) { return s.type_id == id; });
                if (it != systems.end()) {
                    std::cout << it->name << ", ";
                }
            }
            std::cout << "]" << std::endl;
        }
    }

private:
    struct system_desc {
        entt::id_type type_id;
        std::string_view name;
        std::vector<entt::id_type> dependencies;
        std::unique_ptr<system> instance;
    };

    entt::registry& reg;
    std::vector<system_desc> systems{};

    void sort();
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_H
