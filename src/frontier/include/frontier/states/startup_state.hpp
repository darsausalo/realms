#ifndef FRONTIER_STARTUP_STATE_HPP
#define FRONTIER_STARTUP_STATE_HPP

#include <motor/app/state.hpp>
#include <motor/core/event.hpp>

namespace frontier {

class startup_state : public motor::state {
public:
    startup_state(entt::registry& reg);

    motor::transition update() override;

private:
    bool started{};

    void receive_start(const motor::event::start&) { started = true; }
};

} // namespace frontier

#endif // FRONTIER_STARTUP_STATE_HPP
