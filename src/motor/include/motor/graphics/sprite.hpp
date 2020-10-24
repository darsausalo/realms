#ifndef MOTOR_SPRITE_HPP
#define MOTOR_SPRITE_HPP

#include "motor/resources/image.hpp"
#include "motor/resources/image_loader.hpp"

namespace motor {

struct sprite {
    entt::resource_handle<image> image;
};

template<typename Archive>
void serialize(Archive& ar, sprite& value) {
    ar(value.image);
}

} // namespace motor

#endif // MOTOR_SPRITE_HPP
