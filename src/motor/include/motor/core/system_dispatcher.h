#ifndef MOTOR_SYSTEM_DISPATCHER_H
#define MOTOR_SYSTEM_DISPATCHER_H

#include "motor/core/system.h"
#include "motor/core/utility.h"
#include <entt/core/type_info.hpp>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace motor {

class system_dispatcher final {

    template<typename SystemA, typename SystemB>
    inline constexpr void is_same_group() {
        static_assert(SystemA::group == SystemB::group,
                      "System can have dependncy only in same group");
    }

public:
    system_dispatcher(entt::registry& reg) noexcept : reg{reg} {}
    system_dispatcher(system_dispatcher&) = delete;
    system_dispatcher(system_dispatcher&&) = delete;

    system_dispatcher operator=(system_dispatcher&) = delete;
    system_dispatcher operator=(system_dispatcher&&) = delete;

    ~system_dispatcher();

    template<typename System, typename... Dependencies>
    System& add_system() {
        static_assert(std::is_base_of_v<system, System>,
                      "System should be derived from motor::system");
        (is_same_group<System, Dependencies>(), ...);
        auto it = std::find_if(systems.cbegin(), systems.cend(), [](auto&& sd) {
            return sd.type_id == entt::type_info<System>::id();
        });
        systems.push_back(system_desc{entt::type_info<System>::id(),
                                      System::group,
                                      is_host_type_v<System>,
                                      nameof_type<System>(),
                                      {entt::type_info<Dependencies>::id()...},
                                      std::make_unique<System>()});
        systems.back().instance->on_start(reg);
        auto system = static_cast<System*>(systems.back().instance.get());
        need_sorting = true;
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
            need_sorting = true;
        }
    }

    void update();

    std::vector<std::pair<std::string, std::string>> dump();

private:
    struct system_desc {
        entt::id_type type_id;
        system_group group{system_group::sim};
        bool host{};
        std::string_view name;
        std::vector<entt::id_type> dependencies;
        std::unique_ptr<system> instance;
    };

    entt::registry& reg;
    std::vector<system_desc> systems{};
    bool need_sorting{};

    void sort();
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_H
