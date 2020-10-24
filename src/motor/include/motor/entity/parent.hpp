#ifndef MOTOR_PARENT_HPP
#define MOTOR_PARENT_HPP

#include "motor/core/uvec2_sery.hpp"
#include "motor/core/vec2_sery.hpp"
#include <array>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <vector>

namespace motor {

struct parent {
    entt::entity value;
};

template<typename Archive>
void serialize(Archive& ar, parent& value) {
    ar(value.value);
}

} // namespace motor

#endif // MOTOR_PARENT_HPP
