#ifndef MOTOR_TIME_HPP
#define MOTOR_TIME_HPP

#include "motor/core/archive.hpp"

namespace motor {

struct time {
    float delta;
    float elapsed;
};

struct timer {
    float duration;
    float elapsed;
    bool finished;

    void tick(float delta) {
        finished = elapsed >= duration;
        if (!finished) {
            elapsed += delta;
        }
    }

    void reset() {
        elapsed = 0.0f;
        finished = false;
    }
};

template<typename Archive>
void serialize(Archive& ar, timer& value) {
    ar.member(M(value.duration));
}

} // namespace motor

#endif // MOTOR_TIME_HPP
