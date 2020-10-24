#ifndef MOTOR_TILE_SET_HPP
#define MOTOR_TILE_SET_HPP

#include "motor/graphics/image_region.hpp"
#include "motor/resources/image.hpp"
#include <array>
#include <entt/resource/handle.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <sokol_gfx.h>
#include <vector>

namespace motor {

struct tile_set {
    std::vector<entt::resource_handle<image>> tiles;
    std::vector<sg_image> images;
    std::vector<glm::uvec2> dims; // columns,rows
    std::vector<image_region> regions;
    std::vector<std::size_t> indices; // sprite sheet index
    std::vector<std::array<glm::vec2, 4>> rects;
};

template<typename Archive>
void serialize(Archive& ar, tile_set& value) {
    ar(value.tiles);
}

} // namespace motor

#endif // MOTOR_TILE_SET_HPP
