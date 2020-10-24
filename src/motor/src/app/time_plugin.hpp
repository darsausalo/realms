#ifndef MOTOR_TIME_PLUGIN_HPP
#define MOTOR_TIME_PLUGIN_HPP

#include "motor/app/app_builder.hpp"
#include "motor/core/time.hpp"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>

namespace motor {

class time_plugin {
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    time_plugin(app_builder& app)
        : start_point{clock::now()}
        , last_point{clock::now()} {
        app.define_component<timer>();
        app.add_system_to_stage<&time_plugin::update_time>(
            "pre_frame"_hs, *this);
        app.add_system_to_stage<&time_plugin::update_timer>(
            "pre_frame"_hs, *this);
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

} // namespace motor

#endif // MOTOR_TIME_PLUGIN_HPP
