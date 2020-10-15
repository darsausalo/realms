#ifndef MOTOR_SPRITE_PLUGIN_HPP
#define MOTOR_SPRITE_PLUGIN_HPP

#include "motor/entity/rect.hpp"
#include "motor/entity/transform.hpp"
#include "motor/graphics/camera2d.hpp"
#include "motor/graphics/image_atlas.hpp"
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

    void prepare_sprites(entt::view<entt::exclude_t<sg_image>, sprite> view,
                         entt::registry& registry);
    void prepare_sprite_sheets(
        entt::view<entt::exclude_t<sg_image>, sprite_sheet> view,
        entt::registry& registry);
    void update_sprite_sheets(
        entt::view<entt::exclude_t<>, const sprite_sheet, rect> view);
    void emplace_sprites(entt::view<entt::exclude_t<entt::tag<"hidden"_hs>>,
                                    const sg_image,
                                    const transform,
                                    const rect,
                                    const image_atlas> view);
    void render_sprites(const screen& screen, const camera2d& camera);

    void
    render_batch(sg_image image, const sprite_data* sprites, std::size_t count);
};

} // namespace motor

#endif // MOTOR_SPRITE_PLUGIN_HPP
