#ifndef MOTOR_SYSTEM_DISPATCHER_H
#define MOTOR_SYSTEM_DISPATCHER_H

#include "motor/systems/game_data.h"
#include "motor/systems/system.h"
#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace motor {

class system_dispatcher final {
public:
    using system_map = std::unordered_map<entt::id_type, size_t>;
    using system_list = std::vector<std::unique_ptr<system>>;

public:
    system_dispatcher(game_data& data) noexcept : data(data) {}
    ~system_dispatcher();

    template<typename System, typename... Args>
    System& add_system(Args&&... args) {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        system_types.emplace(entt::type_info<System>::id(), systems.size());
        systems.push_back(std::unique_ptr<System>(
                new System(std::forward<Args>(args)...)));
        systems.back()->on_start(data);
        auto system = static_cast<System*>(systems.back().get());
        sort();
        return *system;
    }

    template<typename System>
    void remove_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        if (auto it = system_types.find(entt::type_info<System>::id());
            it != system_types.end()) {
            it->second->on_stop(data);
            systems.erase(systems.begin() + it->second);
            system_types.erase(it);
            sort();
        }
    }

    void update();

private:
    game_data& data;

    system_map system_types{};
    system_list systems{};

    [[maybe_unused]] bool in_update{};

    void sort();
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_H
