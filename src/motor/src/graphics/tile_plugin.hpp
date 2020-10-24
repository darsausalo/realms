#ifndef MOTOR_TILE_PLUGIN_HPP
#define MOTOR_TILE_PLUGIN_HPP

#include "motor/entity/map.hpp"
#include "motor/entity/parent.hpp"
#include "motor/entity/transform.hpp"
#include "motor/graphics/camera2d.hpp"
#include "motor/graphics/screen.hpp"
#include "motor/graphics/tile_chunk.hpp"
#include "motor/graphics/tile_set.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entity/view.hpp>
#include <sokol_gfx.h>
#include <vector>

namespace motor {

class app_builder;

class tile_plugin {
public:
    tile_plugin(app_builder& app);

private:
    struct chunk_data {
        std::size_t layer;
        sg_image image;
        std::size_t start_element{};
        std::size_t element_count{};
    };

    sg_pipeline pipeline{};
    sg_bindings bindings{};
    std::vector<chunk_data> chunks{};

    void update_tilesets(
        entt::view<entt::exclude_t<>, const map, tile_set> view);
    void update_tiles(entt::view<entt::exclude_t<>,
                                 const parent,
                                 const tile_chunk,
                                 const transform> view,
                      const entt::registry& registry);
    void render_tiles(const screen& screen, const camera2d& camera);
};

} // namespace motor

#endif // MOTOR_TILE_PLUGIN_HPP
