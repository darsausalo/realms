#ifndef MOTOR_SPRITE_SHEET_HPP
#define MOTOR_SPRITE_SHEET_HPP

#include "motor/core/archive.hpp"
#include "motor/resources/image.hpp"
#include "motor/resources/image_loader.hpp"

namespace motor {

struct sprite_sheet {
    entt::resource_handle<image> image;
    std::size_t columns;
    std::size_t rows;
    std::size_t index;
};

template<typename Archive>
void serialize(Archive& ar, sprite_sheet& value) {
    ar.member(M(value.image));
    ar.member(M(value.columns));
    ar.member(M(value.rows));
}

} // namespace motor

#endif // MOTOR_SPRITE_SHEET_HPP
