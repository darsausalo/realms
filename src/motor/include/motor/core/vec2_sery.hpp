#ifndef MOTOR_VEC2_SERY_HPP
#define MOTOR_VEC2_SERY_HPP

#include "motor/core/archive.hpp"
#include <glm/ext/vector_float2.hpp>

namespace glm {

template<typename Archive>
void serialize(Archive& ar, glm::vec2& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
}

} // namespace glm

#endif // MOTOR_VEC2_SERY_HPP
