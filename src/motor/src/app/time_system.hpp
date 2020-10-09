#ifndef MOTOR_TIME_SYSTEM_HPP
#define MOTOR_TIME_SYSTEM_HPP

#include "motor/app/app_builder.hpp"
#include "motor/core/archive.hpp"
#include "motor/core/time.hpp"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>

namespace motor {

class time_system {
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    time_system(app_builder& app)
        : start_point{clock::now()}, last_point{clock::now()} {
        app.define_component<timer>();
        app.add_system_to_stage<&time_system::update_time>("pre_frame"_hs,
                                                           *this);
        app.add_system_to_stage<&time_system::update_timer>("pre_frame"_hs,
                                                            *this);
    }

private:
    time_point start_point;
    time_point last_point;

    void update_time(time& game_time) {
        auto now = clock::now();
        game_time.delta =
                std::chrono::duration<float>(now - last_point).count();
        game_time.elapsed =
                std::chrono::duration<float>(now - start_point).count();
        last_point = now;
    }

    void update_timer(entt::view<entt::exclude_t<>, timer> view,
                      const time& game_time) {
        view.each([&game_time](auto& t) { t.tick(game_time.delta); });
    }
};

template<typename Archive>
void serialize(Archive& ar, timer& value) {
    ar.member(M(value.duration));
}

} // namespace motor

#endif
