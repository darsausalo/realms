#include "graphics_plugin.hpp"
#include "graphics/sprite_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <entt/entity/registry.hpp>
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>

namespace motor {

graphics_plugin::graphics_plugin(app_builder& app)
    : screen{app.registry().ctx<motor::screen>()} {
    auto err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error(fmt::format("failed to initialize OpenGL: {}",
                                             glewGetErrorString(err)));
    }

    sg_setup(&sg_desc{});
    assert(sg_isvalid());

    app.add_system_to_stage //
        <&graphics_plugin::pre_render>("pre_render"_hs, *this)
            .add_plugin<sprite_plugin>()
            .add_system_to_stage //
        <&graphics_plugin::post_render>("post_render"_hs, *this);
}

graphics_plugin::~graphics_plugin() { sg_shutdown(); }

void graphics_plugin::pre_render() {
    sg_pass_action pass_action{};
    pass_action.colors[0] = {SG_ACTION_CLEAR, {0.4f, 0.6f, 0.9f, 1.0f}};

    sg_begin_default_pass(&pass_action, screen.width, screen.height);
}

void graphics_plugin::post_render() {
    sg_end_pass();

    sg_commit();
}

} // namespace motor
