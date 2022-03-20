#ifndef SANDBOX_COMPONENTS_H
#define SANDBOX_COMPONENTS_H

#include <motor/core/archive.hpp>

namespace sandbox {

struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

struct health {
    int max;
    int value;
};

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

} // namespace sandbox

#endif // SANDBOX_COMPONENTS_H
