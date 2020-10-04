#ifndef FRONTIER_COMPONENTS_SERIALIZATION_HPP
#define FRONTIER_COMPONENTS_SERIALIZATION_HPP

#include "frontier/components/base.hpp"
#include <motor/core/archive.hpp>
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
    std::string sprite_resource;
    // ar.member("resource", sprite_resource);
    ar(sprite_resource);
    spdlog::debug("sprite_resource: {}", sprite_resource);
    value.resource = sprite_resource;
}

} // namespace frontier

#endif // FRONTIER_COMPONENTS_SERIALIZATION_HPP
