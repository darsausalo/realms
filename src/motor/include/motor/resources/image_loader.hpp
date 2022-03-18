#ifndef MOTOR_IMAGE_LOADER_HPP
#define MOTOR_IMAGE_LOADER_HPP

#include "motor/resources/image.hpp"
#include "motor/resources/resources.hpp"
#include <entt/core/hashed_string.hpp>
#include <entt/resource/handle.hpp>
#include <entt/resource/loader.hpp>
#include <string_view>

namespace motor {

struct image_loader : entt::resource_loader<image_loader, image> {
    std::shared_ptr<image> load(std::string_view name) const {
        return std::make_shared<image>(name);
    }
};

template<typename Archive>
void serialize(Archive& ar, entt::resource_handle<image>& value) {
    std::string name;
    ar(name);
    value = resources::image.load<image_loader>(
        entt::hashed_string{std::data(name)}, name);
}

} // namespace motor

#endif // MOTOR_IMAGE_LOADER_HPP
