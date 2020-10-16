#ifndef MOTOR_APP_BUILDER_HPP
#define MOTOR_APP_BUILDER_HPP

#include "motor/app/app.hpp"
#include <entt/core/type_info.hpp>
#include <spdlog/spdlog.h>

namespace motor {

class app_builder {
    inline static constexpr auto default_stage = "update"_hs;

    friend app;

public:
    using function_type = scheduler::function_type;

    app_builder& add_default_plugins(int argc, const char* argv[]);

    entt::registry& registry() { return app.registry; }

    entt::dispatcher& dispatcher() { return app.dispatcher; }

    prototype_registry& prototypes() { return app.prototypes; }

    template<typename Plugin, typename... Args>
    app_builder& add_plugin(Args&&... args) {
        using namespace std::placeholders;
        auto name = entt::type_id<Plugin>().name();
        auto* plugin = new Plugin(std::forward<Args>(args)..., *this);
        app.plugins.push_back({name, {plugin, [](void* instance) {
                                          delete static_cast<Plugin*>(instance);
                                      }}});
        spdlog::debug("added plugin: {}", name);
        return *this;
    }

    app_builder& add_stage(entt::id_type stage) {
        app.scheduler.add_stage(stage);
        return *this;
    }

    app_builder& add_stage_after(entt::id_type target, entt::id_type stage) {
        app.scheduler.add_stage_after(target, stage);
        return *this;
    }

    app_builder& add_stage_before(entt::id_type target, entt::id_type stage) {
        app.scheduler.add_stage_before(target, stage);
        return *this;
    }

    template<auto Candidate, typename... Req>
    app_builder& add_system_to_stage(entt::id_type stage,
                                     const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Candidate, Req...>(stage, name);
        return *this;
    }

    template<auto Candidate, typename... Req, typename Type>
    app_builder& add_system_to_stage(entt::id_type stage,
                                     Type& value_or_instance,
                                     const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Candidate, Req..., Type>(
            stage, value_or_instance, name);
        return *this;
    }

    template<typename... Req>
    app_builder& add_system_to_stage(entt::id_type stage,
                                     function_type* func,
                                     const void* payload = nullptr,
                                     const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Req...>(stage, func, payload, name);
        return *this;
    }

    template<auto Candidate, typename... Req>
    app_builder& add_system(const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Candidate, Req...>(
            default_stage, name);
        return *this;
    }

    template<auto Candidate, typename... Req, typename Type>
    app_builder& add_system(Type& value_or_instance,
                            const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Candidate, Req..., Type>(
            default_stage, value_or_instance, name);
        return *this;
    }

    template<typename... Req>
    app_builder& add_system(function_type* func,
                            const void* payload = nullptr,
                            const char* name = nullptr) {
        app.scheduler.add_system_to_stage<Req...>(
            default_stage, func, payload, name);
        return *this;
    }

    template<auto Candidate, typename... Req>
    app_builder& add_startup_system(const char* name = nullptr) {
        app.scheduler.add_startup_system<Candidate, Req...>(name);
        return *this;
    }

    template<auto Candidate, typename... Req, typename Type>
    app_builder& add_startup_system(Type& value_or_instance,
                                    const char* name = nullptr) {
        app.scheduler.add_startup_system<Candidate, Req..., Type>(
            value_or_instance, name);
        return *this;
    }

    template<typename... Req>
    app_builder& add_startup_system(function_type* func,
                                    const void* payload = nullptr,
                                    const char* name = nullptr) {
        app.scheduler.add_startup_system<Req...>(func, payload, name);
        return *this;
    }

    template<typename Component,
             component_specifier Specifier = component_specifier::OVERRIDABLE>
    app_builder& define_component() {
        app.prototypes.components.define<Component, Specifier>();
        return *this;
    }

    template<entt::id_type Value,
             component_specifier Specifier = component_specifier::OVERRIDABLE>
    app_builder& define_component() {
        app.prototypes.components.define<Value, Specifier>();
        return *this;
    }

    void run() { app.run(); }

private:
    app app;
};

} // namespace motor

#endif // MOTOR_APP_BUILDER_HPP
