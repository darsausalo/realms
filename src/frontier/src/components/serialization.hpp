#ifndef FRONTIER_COMPONENTS_SERIALIZATION_HPP
#define FRONTIER_COMPONENTS_SERIALIZATION_HPP

#include "frontier/components/base.hpp"
#include <entt/core/hashed_string.hpp>
#include <motor/core/archive.hpp>
#include <motor/resources/image_loader.hpp>
#include <motor/resources/resources.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

template<typename Archive>
void serialize(Archive& ar, position& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
}

template<typename Archive>
void serialize(Archive& ar, velocity& value) {
    ar.member(M(value.dx));
    ar.member(M(value.dy));
}

template<typename Archive>
void serialize(Archive& ar, health& value) {
    ar.member(M(value.max));
    ar.member(M(value.value));
}

template<typename Archive>
void serialize(Archive& ar, sprite& value) {
    std::string name;
    ar(name);
    spdlog::debug("sprite_resource: {}", name);
    value.image = motor::resources::image.load<motor::image_loader>(
            entt::hashed_string{std::data(name)}, name);
}

} // namespace frontier

#endif // FRONTIER_COMPONENTS_SERIALIZATION_HPP
