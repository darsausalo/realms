#ifndef MOTOR_CAMERA2D_HPP
#define MOTOR_CAMERA2D_HPP

#include <glm/ext/vector_float2.hpp>

namespace motor {

struct camera2d {
    glm::vec2 position;
    float zoom;
};

} // namespace motor

#endif // MOTOR_CAMERA2D_HPP
