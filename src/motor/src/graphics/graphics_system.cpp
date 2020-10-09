#include "graphics_system.hpp"
#include "motor/app/app_builder.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <cassert>
#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>

namespace motor {

graphics_system::graphics_system(app_builder& app)
    : screen{app.registry().ctx<motor::screen>()} {
    auto err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error(fmt::format("failed to initialize OpenGL: {}",
                                             glewGetErrorString(err)));
    }

    sg_setup(&sg_desc{});
    assert(sg_isvalid());

    app.add_system_to_stage<&graphics_system::update>("post_frame"_hs, *this);
}

graphics_system::~graphics_system() {
    sg_shutdown();
}

void graphics_system::update() {
    sg_pass_action pass_action{};
    pass_action.colors[0] = {SG_ACTION_CLEAR, {0.4f, 0.6f, 0.9f, 1.0f}};

    sg_begin_default_pass(&pass_action, screen.width, screen.height);
    sg_end_pass();

    sg_commit();
}

} // namespace motor
