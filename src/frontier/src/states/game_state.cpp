#include "frontier/states/game_state.hpp"
#include "frontier/components/base.hpp"
#include <chrono>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

struct test_system {
    entt::registry& registry;
    std::chrono::steady_clock::time_point last_time{};

    test_system(entt::registry& registry) : registry{registry} {
        spdlog::debug("test_system::start");
    }
    ~test_system() { spdlog::debug("test_system::stop"); }

    void operator()() {
        using namespace std::chrono_literals;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<float>(now - last_time);
        if (duration >= 1s) {
            spdlog::debug("<<< tick >>>");
            last_time = now;

            registry.view<position>().each([](auto& p) {
                p.x += 1;
                p.y += 1;
                spdlog::debug("p = {},{}", p.x, p.y);
            });

            registry.view<health>().each([](const auto& h) {
                spdlog::debug("h = {},{}", h.max, h.value);
            });
        }
    }
};

void game_state::on_start() {
    registry.clear();

    add_system<motor::system_group::on_update, test_system>(registry);

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
