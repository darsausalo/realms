#ifndef MOTOR_IMAGE_LOADER_HPP
#define MOTOR_IMAGE_LOADER_HPP

#include "motor/resources/image.hpp"
#include <entt/resource/loader.hpp>
#include <string_view>

namespace motor {

struct image_loader : entt::resource_loader<image_loader, image> {
    std::shared_ptr<image> load(std::string_view name) const {
        return std::make_shared<image>(name);
    }
};

} // namespace motor

#endif // MOTOR_IMAGE_LOADER_HPP
