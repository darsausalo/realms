#ifndef MOTOR_TILE_SET_HPP
#define MOTOR_TILE_SET_HPP

#include "motor/resources/image.hpp"
#include <entt/resource/handle.hpp>
#include <vector>

namespace motor {

struct tile_set {
    std::vector<entt::resource_handle<image>> value;
};

template<typename Archive>
void serialize(Archive& ar, tile_set& value) {
    ar(value.value);
}

} // namespace motor

#endif // MOTOR_TILE_SET_HPP
