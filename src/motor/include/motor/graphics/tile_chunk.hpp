#ifndef MOTOR_TILE_CHUNK_HPP
#define MOTOR_TILE_CHUNK_HPP

#include "motor/core/archive.hpp"
#include <vector>

namespace motor {

struct tile_chunk {
    std::size_t layer;
    std::vector<std::uint16_t> tiles{};
};

template<typename Archive>
void serialize(Archive& ar, tile_chunk& value) {
    ar.member(M(value.layer));
    ar.member(M(value.tiles));
}

} // namespace motor

#endif // MOTOR_TILE_CHUNK_HPP
