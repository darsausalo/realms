#ifndef MOTOR_MAP_HPP
#define MOTOR_MAP_HPP

#include "motor/core/archive.hpp"
#include "motor/core/uvec2_sery.hpp"
#include "motor/core/vec2_sery.hpp"

namespace motor {

struct map {
    glm::vec2 tile_size;
    glm::uvec2 chunk_size;
};

template<typename Archive>
void serialize(Archive& ar, map& value) {
    ar.member(M(value.tile_size));
    ar.member(M(value.chunk_size));
}

} // namespace motor

#endif // MOTOR_MAP_HPP
