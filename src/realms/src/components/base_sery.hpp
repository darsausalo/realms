#ifndef REALMS_COMPONENTS_BASE_SERY_HPP
#define REALMS_COMPONENTS_BASE_SERY_HPP

#include "base.hpp"
#include <motor/core/archive.hpp>

namespace realms {

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
void serialize(Archive& ar, speed& value) {
    ar.member(M(value.value));
}

} // namespace realms

#endif // REALMS_COMPONENTS_BASE_SERY_HPP
