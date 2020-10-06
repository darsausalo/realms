#include "frontier/states/game_state.hpp"
#include "frontier/components/base.hpp"
#include <entt/signal/dispatcher.hpp>
#include <motor/core/time.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

struct test_system {
    entt::registry& registry;
    motor::time& time;
    motor::timer timer;

    test_system(entt::registry& registry)
        : registry{registry}, time{registry.ctx<motor::time>()}, timer{1.0f,
                                                                       0.0f} {
        spdlog::debug("test_system::start");
    }
    ~test_system() { spdlog::debug("test_system::stop"); }

    void operator()() {
        timer.tick(time.delta);
        if (timer.finished) {
            timer.reset();

            spdlog::debug("<<< tick >>>");

            registry.view<const health>().each([](const auto& h) {
                spdlog::debug("h = {},{}", h.max, h.value);
            });
        }

        registry.view<motor::timer, position>().each([](auto& t, auto& p) {
            if (t.finished) {
                t.reset();
                p.x += 1;
                p.y += 1;
                spdlog::debug("p = {},{}", p.x, p.y);
            }
        });
    }
};

void game_state::on_start() {
    registry.clear();

    add_system<test_system>(registry);

    registry.ctx<motor::prototype_registry>().spawn(registry, "soldier2"_hs);

    dispatcher.sink<motor::event::respawn>()
            .connect<&game_state::receive_respawn>(*this);

    spdlog::debug("game: start");
}

void game_state::on_stop() {
    dispatcher.sink<motor::event::respawn>().disconnect(*this);
    spdlog::debug("game: stop");
}

motor::transition game_state::update() {
    return motor::transition_none{};
}

void game_state::receive_respawn(const motor::event::respawn&) {
    prototype_registry.respawn(registry);
}

} // namespace frontier
