#include "motor/app/app_builder.hpp"
#include "app/config_plugin.hpp"
#include "app/input_plugin.hpp"
#include "app/time_plugin.hpp"
#include "app/window_plugin.hpp"
#include "entity/map_plugin.hpp"
#include "graphics/graphics_plugin.hpp"
#include "gui/gui_plugin.hpp"
#include "mods/mods_plugin.hpp"
#include "motor/core/input.hpp"
#include <spdlog/spdlog.h>

namespace motor {

void print_debug_info(const entt::registry& registry) {
    if (registry.ctx<input_actions>().is_just_pressed("print_debug_info"_hs)) {
        std::string info{};

        std::size_t cnt = 0u;
        registry.ctx([&cnt, &info](auto& ti) {
            info += fmt::format("    ctx var: {}\n", ti.name());
            cnt++;
        });
        info += fmt::format("  ctx var count: {}", cnt);

        spdlog::debug("registry:\n{}", info);
    }
}

app_builder& app_builder::add_default_plugins(int argc, const char* argv[]) {
    return add_plugin<config_plugin>(arg_list{argc, argv})
        .add_plugin<time_plugin>()
        .add_plugin<map_plugin>()
        .add_plugin<window_plugin>()
        .add_plugin<input_plugin>()
        .add_plugin<mods_plugin>()
        .add_plugin<graphics_plugin>()
        .add_plugin<gui_plugin>()
        .add_system_to_stage<&print_debug_info>("post_frame"_hs);
}

} // namespace motor
