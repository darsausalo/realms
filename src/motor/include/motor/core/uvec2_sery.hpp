#ifndef MOTOR_UVEC2_SERY_HPP
#define MOTOR_UVEC2_SERY_HPP

#include "motor/core/archive.hpp"
#include <glm/ext/vector_uint2.hpp>

namespace glm {

template<typename Archive>
void serialize(Archive& ar, glm::uvec2& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
}

} // namespace glm

#endif // MOTOR_UVEC2_SERY_HPP
