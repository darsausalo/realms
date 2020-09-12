#include "frontier/components.h"
#include <entt/entity/registry.hpp>
#include <motor/core/plugin_context.h>
#include <motor/core/system.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

namespace frontier {

class test_system : public motor::system<motor::system_group::on_update> {
public:
    void on_start(entt::registry& reg) override {
        spdlog::info("test_system: start[2]: {}",
                     reg.view<position, velocity>().size());
    }
    void on_stop(entt::registry& reg) override {
        spdlog::info("test_system: stop");
    }

    void update(entt::registry& reg) override {}
};

MOTOR_EXPORT void plugin_entry(motor::plugin_context* ctx) {
    spdlog::set_default_logger(ctx->get_logger());

    ctx->system<test_system>();

    spdlog::debug("plugin_entry: core[1]");
}

} // namespace frontier
