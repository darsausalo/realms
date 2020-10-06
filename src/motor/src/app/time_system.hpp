#ifndef MOTOR_TIME_SYSTEM_HPP
#define MOTOR_TIME_SYSTEM_HPP

#include "motor/core/archive.hpp"
#include "motor/core/time.hpp"
#include <chrono>
#include <entt/entity/registry.hpp>

namespace motor {

class time_system {
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    time_system(entt::registry& registry)
        : game_time{registry.ctx_or_set<time>()}, start_point{clock::now()},
          last_point{clock::now()} {}

    void operator()() {
        auto now = clock::now();
        game_time.delta =
                std::chrono::duration<float>(now - last_point).count();
        game_time.elapsed =
                std::chrono::duration<float>(now - start_point).count();
        last_point = now;
    }

private:
    time& game_time;
    time_point start_point;
    time_point last_point;
};

class timer_system {
public:
    timer_system(entt::registry& registry)
        : registry{registry}, game_time{registry.ctx_or_set<time>()} {}

    void operator()() {
        registry.view<timer>().each(
                [this](auto& t) { t.tick(game_time.delta); });
    }

private:
    entt::registry& registry;
    const time& game_time;
};

template<typename Archive>
void serialize(Archive& ar, timer& value) {
    ar.member(M(value.duration));
}

} // namespace motor

#endif