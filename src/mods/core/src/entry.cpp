#include "frontier/components/base.h"
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
        auto e = reg.create();
        reg.emplace<position>(e, 11.0f, 12.0f);
        reg.emplace<velocity>(e, 13.0f, 14.0f);

        entt::entity e2{};

        for (auto i = 0u; i < 10000; ++i) {
            e2 = reg.create();
        }

        reg.emplace<position>(e2, 15.0f, 16.0f);
        reg.emplace<velocity>(e2, 17.0f, 18.0f);

        spdlog::info("test_system: start[1]: {}, valid: {}",
                     reg.view<position, velocity>().size(), reg.valid(e2));

        reg.view<position, velocity>().each([](const auto& p, const auto& v) {
            spdlog::debug("p={},{}; v={},{}", p.x, p.y, v.dx, v.dy);
        });

        spdlog::debug("test_system: position::type_id: {}, has_ti: {}",
                      entt::type_info<position>::id(),
                      entt::has_type_index_v<position>);
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
