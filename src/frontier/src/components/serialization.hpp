#ifndef FRONTIER_COMPONENTS_SERIALIZATION_HPP
#define FRONTIER_COMPONENTS_SERIALIZATION_HPP

#include "frontier/components/base.hpp"
#include <motor/core/archive.hpp>

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

} // namespace frontier

#endif // FRONTIER_COMPONENTS_SERIALIZATION_HPP
