#ifndef MOTOR_SPRITE_PLUGIN_HPP
#define MOTOR_SPRITE_PLUGIN_HPP

#include "motor/entity/transform.hpp"
#include "motor/graphics/camera2d.hpp"
#include "motor/graphics/image_region.hpp"
#include "motor/graphics/screen.hpp"
#include "motor/graphics/sprite.hpp"
#include "motor/graphics/sprite_sheet.hpp"
#include <array>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/view.hpp>
#include <sokol_gfx.h>
#include <vector>

namespace motor {

using namespace entt::literals;

class app_builder;

class sprite_plugin {
public:
    sprite_plugin(app_builder& app);

private:
    struct sprite_data {
        sg_image image;
        glm::mat4 tfm;
        std::array<glm::vec2, 4> rect;
        glm::vec2 atlas_size;
    };

    sg_pipeline pipeline{};
    sg_bindings bindings{};

    std::vector<sprite_data> sprites;

    void prepare_sprites(
        entt::view<entt::get_t<sprite>, entt::exclude_t<sg_image>> view,
        entt::registry& registry);
    void prepare_sprite_sheets(
        entt::view<entt::get_t<sprite_sheet>, entt::exclude_t<sg_image>> view,
        entt::registry& registry);
    void update_sprite_sheets(
        entt::view<entt::get_t<const sprite_sheet, image_region>> view);
    void emplace_sprites(
        entt::view<
            entt::get_t<const sg_image, const transform, const image_region>,
            entt::exclude_t<entt::tag<"hidden"_hs>>> view);
    void render_sprites(const screen& screen, const camera2d& camera);

    void render_batch(sg_image image,
                      const sprite_data* sprites,
                      std::size_t count,
                      std::size_t& base_element);
};

} // namespace motor

#endif // MOTOR_SPRITE_PLUGIN_HPP
