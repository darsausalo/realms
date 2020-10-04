#include "frontier/states/game_state.hpp"
#include <spdlog/spdlog.h>

namespace frontier {

struct test_system {
    test_system() { spdlog::debug("test_system::start"); }
    ~test_system() { spdlog::debug("test_system::stop"); }
};

game_state::game_state(entt::registry& reg) : motor::state{reg} {
    add_system<motor::system_group::on_update, test_system>();
    spdlog::debug("game: start");
}

game_state::~game_state() {
    spdlog::debug("game: stop");
}

motor::transition game_state::update() {
    return motor::transition_none{};
}

} // namespace frontier
