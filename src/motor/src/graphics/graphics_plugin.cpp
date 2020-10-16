#include "graphics_plugin.hpp"
#include "graphics/sprite_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/events.hpp"
#include "motor/graphics/rect_packer.hpp"
#include "motor/resources/image_atlas.hpp"
#include "motor/resources/resources.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include <sokol_gfx.h>

namespace motor {

void to_json(nlohmann::json& j, const graphics_config& c) {
    j = nlohmann::json{{"max_texture_size", c.max_texture_size}};
}

void from_json(const nlohmann::json& j, graphics_config& c) {
    j.at("max_texture_size").get_to(c.max_texture_size);
}

graphics_plugin::graphics_plugin(app_builder& app)
    : screen{app.registry().ctx<motor::screen>()}
    , jconfig{app.registry().ctx<nlohmann::json>()}
    , config{8 * 1024} {
    try {
        jconfig.at("graphics").get_to(config);
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid graphics config: {}", e.what());
    }

    auto err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error(fmt::format(
            "failed to initialize OpenGL: {}", glewGetErrorString(err)));
    }

    sg_setup(&sg_desc{});
    assert(sg_isvalid());

    app.add_system_to_stage<&graphics_plugin::pre_render>(
           "pre_render"_hs, *this)
        .add_plugin<sprite_plugin>()
        .add_system_to_stage<&graphics_plugin::post_render>(
            "post_render"_hs, *this);

    app.dispatcher()
        .sink<event::start>()
        .connect<&graphics_plugin::build_atlases>(*this);
}

graphics_plugin::~graphics_plugin() { sg_shutdown(); }

void graphics_plugin::build_atlases() {
    // TODO: move to startup system
    std::vector<entt::resource_handle<image>> images;
    resources::image.each(
        [&images](auto&&, auto&& image) { images.push_back(image); });

    const auto limits = sg_query_limits();
    const auto max_texture_size =
        std::min(config.max_texture_size, limits.max_image_size_2d);
    rect_packer packer{max_texture_size, max_texture_size, 1};
    for (std::size_t i = 0; i < images.size(); ++i) {
        packer.emplace(i, images[i]->width(), images[i]->height());
    }

    while (!packer.empty()) {
        auto rects = packer.pack();
        if (!rects.empty()) {
            auto atlas = std::make_shared<image_atlas>(
                max_texture_size, max_texture_size);
            for (auto&& [id, rect] : rects) {
                images[id]->stamp_into(atlas, rect);
            }
            atlases.push_back(atlas);
        } else {
            std::string msg{"images not fit in atlases:\n"};
            for (auto&& image : images) {
                if (!image->valid()) {
                    msg +=
                        fmt::format("  {}x{}", image->width(), image->height());
                }
            }
            spdlog::error(msg);
            break;
        }
    }

    for (auto&& atlas : atlases) {
        atlas->upload();
    }
}

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
