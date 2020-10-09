#include "motor/app/app_builder.hpp"
#include "app/config_system.hpp"
#include "app/input_system.hpp"
#include "app/time_system.hpp"
#include "app/window_system.hpp"
#include "graphics/graphics_system.hpp"
#include "mods/mods_system.hpp"
#include "motor/core/input.hpp"
#include <spdlog/spdlog.h>

namespace motor {

void print_debug_info(const entt::registry& registry) {
    if (registry.ctx<input_actions>().is_just_pressed("print_debug_info"_hs)) {
        std::size_t cnt = 0u;
        registry.ctx([&cnt](auto& ti) {
            spdlog::debug("ctx var: {}", ti.name());
            cnt++;
        });
        spdlog::debug("ctx var count: {}", cnt);
    }
}

app_builder& app_builder::add_default_plugins(int argc, const char* argv[]) {
    return add_plugin<config_system>(arg_list{argc, argv})
            .add_plugin<time_system>()
            .add_plugin<window_system>()
            .add_plugin<input_system>()
            .add_plugin<mods_system>()
            .add_plugin<graphics_system>()
            .add_system<&print_debug_info>("post_frame"_hs);
}

} // namespace motor
