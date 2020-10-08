#include "graphics_system.hpp"
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

graphics_system::graphics_system(entt::registry& registry)
    : registry{registry}, screen{registry.ctx_or_set<motor::screen>()} {
    auto err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error(fmt::format("failed to initialize OpenGL: {}",
                                             glewGetErrorString(err)));
    }

    sg_setup(&sg_desc{});
    assert(sg_isvalid());

    spdlog::debug("graphics_system::start");
}

graphics_system::~graphics_system() {
    spdlog::debug("graphics_system::stop");
    sg_shutdown();
}

void graphics_system::operator()() {
    sg_pass_action pass_action{};
    pass_action.colors[0] = {SG_ACTION_CLEAR, {0.0f, 0.0f, 0.0f, 1.0f}};

    sg_begin_default_pass(&pass_action, screen.width, screen.height);
    sg_end_pass();

    sg_commit();
}

} // namespace motor
