#ifndef MOTOR_IMAGE_REGION_HPP
#define MOTOR_IMAGE_REGION_HPP

#include <glm/ext/vector_float2.hpp>

namespace motor {

struct image_region {
    glm::vec2 origin;
    glm::vec2 size;
    glm::vec2 atlas_origin;
    glm::vec2 atlas_size;
};

} // namespace motor

#endif // MOTOR_IMAGE_REGION_HPP
