#ifndef MOTOR_IVEC3_SERY_HPP
#define MOTOR_IVEC3_SERY_HPP

#include "motor/core/archive.hpp"
#include <glm/ext/vector_int2.hpp>

namespace glm {

template<typename Archive>
void serialize(Archive& ar, glm::ivec3& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
    ar.member(M(value.z));
}

} // namespace glm

#endif // MOTOR_IVEC3_SERY_HPP
