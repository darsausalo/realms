#ifndef MOTOR_IVEC2_SERY_HPP
#define MOTOR_IVEC2_SERY_HPP

#include "motor/core/archive.hpp"
#include <glm/ext/vector_int2.hpp>

namespace glm {

template<typename Archive>
void serialize(Archive& ar, glm::ivec2& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
}

} // namespace glm

#endif // MOTOR_IVEC2_SERY_HPP
