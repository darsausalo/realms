#ifndef MOTOR_SYSTEM_DISPATCHER_HPP
#define MOTOR_SYSTEM_DISPATCHER_HPP

#include <entt/core/type_info.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace motor {

enum class stage {
    NONE,
    PRE_FRAME,
    ON_EVENT,
    PRE_UPDATE,
    ON_UPDATE,
    POST_UPDATE,
    POST_FRAME
};

class system_dispatcher final {
public:
    system_dispatcher() noexcept = default;
    system_dispatcher(system_dispatcher&) = delete;
    system_dispatcher(system_dispatcher&&) = delete;

    system_dispatcher operator=(system_dispatcher&) = delete;
    system_dispatcher operator=(system_dispatcher&&) = delete;

    ~system_dispatcher() noexcept;

    template<typename System, stage Stage = stage::ON_UPDATE, typename... Args>
    auto add(Args&&... args) {
        auto type_id = entt::type_info<System>::id();
        assert(std::find_if(systems.cbegin(), systems.cend(),
                            [type_id](auto&& system) {
                                return system.type_id == type_id;
                            }) == systems.cend());
        auto* system = new System{std::forward<Args>(args)...};
        if constexpr (std::is_invocable_v<System>) {
            static_assert(Stage != stage::NONE);
            systems.push_back({type_id,
                               Stage,
                               std::bind(&System::operator(), system),
                               {system, [](void* instance) {
                                    delete static_cast<System*>(instance);
                                }}});
        } else {
            static_assert(Stage == stage::NONE);
            systems.push_back(
                    {type_id, Stage, nullptr, {system, [](void* instance) {
                                                   delete static_cast<System*>(
                                                           instance);
                                               }}});
        }
        sort();
        return type_id;
    }

    template<typename Func, stage Stage = stage::ON_UPDATE>
    auto add(Func func) {
        static_assert(Stage != stage::NONE);
        auto type_id = entt::type_info<decltype(func)>::id();
        assert(std::find_if(systems.cbegin(), systems.cend(),
                            [type_id](auto&& system) {
                                return system.type_id == type_id;
                            }) == systems.cend());
        systems.push_back({type_id, Stage, func, {nullptr, nullptr}});
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
        for (auto&& order : exec_order) {
            func(systems[order].type_id);
        }
    }

private:
    struct system {
        entt::id_type type_id;
        stage stage;
        std::function<void()> function;
        std::unique_ptr<void, void (*)(void*)> instance;
    };

    std::vector<system> systems{};
    std::vector<std::size_t> exec_order{};

    void sort();
};

} // namespace motor

#endif // MOTOR_SYSTEM_DISPATCHER_HPP
