#ifndef MOTOR_SYSTEM_DISPATCHER_H
#define MOTOR_SYSTEM_DISPATCHER_H

#include "motor/systems/system.h"
#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace motor {

class game_data;

class system_dispatcher final {
public:
    using system_map = std::unordered_map<entt::id_type, size_t>;
    using system_list = std::vector<std::unique_ptr<system>>;

public:
    system_dispatcher() noexcept = default;
    ~system_dispatcher() noexcept = default;

    template<typename System>
    void add_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        system_types.emplace(entt::type_info<System>::id(), systems.size());
        systems.push_back(std::make_unique<System>());
    }

    template<typename System>
    void remove_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        if (auto it = system_types.find(entt::type_info<System>::id());
            it != system_types.end()) {
            systems.erase(systems.begin() + it->second);
            system_types.erase(it);
        }
    }

    void start(game_data& data);
    void stop(game_data& data);
    void update(game_data& data);

private:
    system_map system_types{};
    system_list systems{};
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_H
