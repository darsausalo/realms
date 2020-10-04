#ifndef MOTOR_SYSTEM_DISPATCHER_H
#define MOTOR_SYSTEM_DISPATCHER_H

#include <entt/core/type_info.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace motor {

enum class system_group {
    pre_frame,
    on_load,
    post_load,
    pre_update,
    on_update,
    on_validate,
    post_update,
    pre_store,
    on_store,
    post_frame
};

class system_dispatcher final {
public:
    system_dispatcher() noexcept = default;
    system_dispatcher(system_dispatcher&) = delete;
    system_dispatcher(system_dispatcher&&) = delete;

    system_dispatcher operator=(system_dispatcher&) = delete;
    system_dispatcher operator=(system_dispatcher&&) = delete;

    ~system_dispatcher() noexcept;

    template<system_group Group, typename System, typename... Args>
    auto add(Args&&... args) {
        auto type_id = entt::type_info<System>::id();
        assert(std::find_if(systems.cbegin(), systems.cend(),
                            [type_id](auto&& system) {
                                return system.type_id == type_id;
                            }) == systems.cend());
        auto* system = new System{std::forward<Args>(args)...};
        if constexpr (std::is_invocable_v<System>) {
            systems.push_back({type_id,
                               Group,
                               std::bind(&System::operator(), system),
                               {system, [](void* instance) {
                                    delete static_cast<System*>(instance);
                                }}});
        } else {
            systems.push_back(
                    {type_id, Group, nullptr, {system, [](void* instance) {
                                                   delete static_cast<System*>(
                                                           instance);
                                               }}});
        }
        sort();
        return type_id;
    }

    template<system_group Group, typename Func>
    auto add(Func func) {
        auto type_id = entt::type_info<decltype(func)>::id();
        assert(std::find_if(systems.cbegin(), systems.cend(),
                            [type_id](auto&& system) {
                                return system.type_id == type_id;
                            }) == systems.cend());
        systems.push_back({type_id, Group, func, {nullptr, nullptr}});
        sort();
        return type_id;
    }

    auto remove(entt::id_type type_id) {
        systems.erase(std::remove_if(systems.begin(), systems.end(),
                                     [type_id](auto&& system) {
                                         return system.type_id == type_id;
                                     }),
                      systems.end());
        sort();
    }

    void update();

    template<typename Func>
    void visit(Func func) {
        for (auto&& desc : systems) {
            func(desc.type_id);
        }
    }

private:
    struct system {
        entt::id_type type_id;
        system_group group;
        std::function<void()> function;
        std::unique_ptr<void, void (*)(void*)> instance;

        void operator()() {
            if (function) {
                function();
            }
        }
    };

    std::vector<system> systems{};

    void sort();
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_H
