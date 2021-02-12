#ifndef MOTOR_GRAPHICS_PIPELINE_HPP
#define MOTOR_GRAPHICS_PIPELINE_HPP

#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <sokol_gfx.h>

namespace motor {

inline constexpr const std::size_t max_batch_size = 1024 * 16;
inline constexpr const std::size_t max_vertices = max_batch_size * 4;
inline constexpr const std::size_t max_indices = max_batch_size * 6;

struct vertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
};

struct vs_params_t {
    glm::vec2 screen_size;
    glm::vec2 camera_position;
    float camera_zoom;
};

void init_pipeline(sg_pipeline& pipeline, sg_bindings& bindings);

} // namespace motor

#endif // MOTOR_GRAPHICS_PIPELINE_HPP
