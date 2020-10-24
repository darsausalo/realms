#ifndef MOTOR_UVEC3_SERY_HPP
#define MOTOR_UVEC3_SERY_HPP

#include "motor/core/archive.hpp"
#include <glm/ext/vector_uint2.hpp>

namespace glm {

template<typename Archive>
void serialize(Archive& ar, glm::uvec3& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
    ar.member(M(value.z));
}

} // namespace glm

#endif // MOTOR_UVEC3_SERY_HPP
