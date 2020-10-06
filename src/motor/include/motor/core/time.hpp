#ifndef MOTOR_TIME_HPP
#define MOTOR_TIME_HPP

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

} // namespace motor

#endif // MOTOR_TIME_HPP
