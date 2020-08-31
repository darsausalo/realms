#ifndef MOTOR_CONTEXT_H
#define MOTOR_CONTEXT_H

#include <cassert>
#include <entt/core/type_info.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <tsl/hopscotch_map.h>

namespace motor {

class context {
public:
    entt::registry registry{};

    context() noexcept = default;
    context(const context&) noexcept = default;
    context(context&&) noexcept = default;
    ~context() noexcept = default;

    template<typename Type, typename... Args>
    Type& set(Args&&... args) {
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

    template<typename Type>
    const Type& get() const {
        auto* v = get_if();
        assert(v);
        return *v;
    }

    template<typename Type>
    Type& get() {
        auto* v =
                const_cast<Type*>(std::as_const(*this).template get_if<Type>());
        assert(v);
        return *v;
    }

private:
    using variable_data = std::unique_ptr<void, void (*)(void*)>;

    tsl::hopscotch_map<entt::id_type, variable_data> vars{};
};

} // namespace motor

#endif // MOTOR_CONTEXT_H
