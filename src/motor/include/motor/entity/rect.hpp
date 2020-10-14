#ifndef MOTOR_RECT_HPP
#define MOTOR_RECT_HPP

#include <glm/ext/vector_float2.hpp>

namespace motor {

struct rect {
    glm::vec2 min;
    glm::vec2 max;
};

} // namespace motor

#endif // MOTOR_RECT_HPP
