#ifndef MOTOR_GAME_DATA_H
#define MOTOR_GAME_DATA_H

#include <entt/core/type_info.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <unordered_map>

namespace motor {

class game_data {
public:
    entt::dispatcher event_dispatcher{};
    entt::registry registry{};
    entt::registry prefab_registry{};

public:
    game_data() noexcept = default;
    ~game_data() noexcept = default;

    template<typename Type, typename... Args>
    Type& set(Args&&... args) {
        unset<Type>();
        auto [it, _] = vars.insert_or_assign(
                entt::type_info<Type>::id(),
                std::unique_ptr<void, void (*)(void*)>{
                        new Type{std::forward<Args>(args)...},
                        [](void* instance) {
                            delete static_cast<Type*>(instance);
                        }});
        return *static_cast<Type*>(it->second.get());
    }

    template<typename Type>
    void unset() {
        vars.erase(entt::type_info<Type>::id());
    }

    template<typename Type>
    const Type* get_if() const {
        if (auto it = vars.find(entt::type_info<Type>::id());
            it != vars.end()) {
            return static_cast<Type*>(it->second.get());
        }
        return nullptr;
    }

    template<typename Type>
    Type* get_if() {
        return const_cast<Type*>(std::as_const(*this).template get_if<Type>());
    }

private:
    using variable_data = std::unique_ptr<void, void (*)(void*)>;

    std::unordered_map<entt::id_type, variable_data> vars{};
};

} // namespace motor

#endif // MOTOR_GAME_DATA_H
