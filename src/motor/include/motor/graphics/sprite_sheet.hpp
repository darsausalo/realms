#ifndef MOTOR_SPRITE_SHEET_HPP
#define MOTOR_SPRITE_SHEET_HPP

#include "motor/core/archive.hpp"
#include "motor/resources/image.hpp"
#include "motor/resources/image_loader.hpp"
#include "motor/resources/resources.hpp"
#include <entt/resource/handle.hpp>

namespace motor {

struct sprite_sheet {
    entt::resource_handle<image> image;
    std::size_t columns;
    std::size_t rows;
    std::size_t index;
};

template<typename Archive>
void serialize(Archive& ar, sprite_sheet& value) {
    std::string name;
    ar.member("image", name);
    ar.member(M(value.columns));
    ar.member(M(value.rows));
    value.image = resources::image.load<image_loader>(
        entt::hashed_string{std::data(name)}, name);
}

} // namespace motor

#endif // MOTOR_SPRITE_SHEET_HPP
