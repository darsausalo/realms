#ifndef MOTOR_SPRITE_HPP
#define MOTOR_SPRITE_HPP

#include "motor/resources/image.hpp"
#include "motor/resources/image_loader.hpp"
#include "motor/resources/resources.hpp"
#include <entt/resource/handle.hpp>

namespace motor {

struct sprite {
    entt::resource_handle<image> image;
};

template<typename Archive>
void serialize(Archive& ar, sprite& value) {
    std::string name;
    ar(name);
    value.image = resources::image.load<image_loader>(
        entt::hashed_string{std::data(name)}, name);
}

} // namespace motor

#endif // MOTOR_SPRITE_HPP
