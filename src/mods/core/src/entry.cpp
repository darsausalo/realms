#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <motor/core/attribute.h>
#include <motor/core/plugin_context.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

namespace core {

class test_system : public motor::system {
public:
    void on_start(entt::registry& reg) override {
        spdlog::info("test_system: start[1]");
    }
    void on_stop(entt::registry& reg) override {
        spdlog::info("test_system: stop");
    }

    void update(entt::registry& reg) override {}
};

class core_module : public motor::system_module {
public:
    core_module() { system<test_system>(); }
};

} // namespace core

REFL_AUTO(type(core::test_system));
REFL_AUTO(type(core::core_module));

namespace core {

MOTOR_EXPORT void plugin_entry(motor::plugin_context* ctx) {
    spdlog::set_default_logger(ctx->get_logger());

    ctx->module<core_module>();

    spdlog::debug("plugin_entry: core[1]");
}

} // namespace core
