#include "frontier/states/main_state.h"
#include <spdlog/spdlog.h>

namespace frontier {

struct test_system {
    test_system() { spdlog::debug("test_system::start"); }
    ~test_system() { spdlog::debug("test_system::stop"); }
};

main_state::main_state(entt::registry& reg) : motor::state{reg} {
    add_system<motor::system_group::on_update, test_system>();
    spdlog::debug("game: start");
}

main_state::~main_state() {
    spdlog::debug("game: stop");
}

motor::transition main_state::update() {
    return motor::transition_none{};
}

} // namespace frontier
