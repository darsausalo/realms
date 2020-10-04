#include "frontier/states/game_state.hpp"
#include "frontier/components/base.hpp"
#include <chrono>
#include <entt/signal/dispatcher.hpp>
#include <motor/entity/prototype_registry.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

struct test_system {
    entt::registry& reg;
    std::chrono::steady_clock::time_point last_time{};

    test_system(entt::registry& reg) : reg{reg} {
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

            reg.view<position>().each([](auto& p) {
                p.x += 1;
                p.y += 1;
                spdlog::debug("p = {},{}", p.x, p.y);
            });

            reg.view<health>().each([](const auto& h) {
                spdlog::debug("h = {},{}", h.max, h.value);
            });
        }
    }
};

game_state::game_state(entt::registry& reg) : motor::state{reg} {
    add_system<motor::system_group::on_update, test_system>(world_reg);
    spdlog::debug("game: start");

    reg.ctx<motor::prototype_registry>().spawn(world_reg, "soldier2"_hs);

    reg.ctx<entt::dispatcher>()
            .sink<motor::event::respawn>()
            .connect<&game_state::receive_respawn>(*this);
}

game_state::~game_state() {
    spdlog::debug("game: stop");
}

motor::transition game_state::update() {
    return motor::transition_none{};
}

void game_state::receive_respawn(const motor::event::respawn&) {
    reg.ctx<motor::prototype_registry>().respawn(world_reg);
}

} // namespace frontier
