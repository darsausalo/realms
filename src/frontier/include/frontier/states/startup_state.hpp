#ifndef FRONTIER_STARTUP_STATE_HPP
#define FRONTIER_STARTUP_STATE_HPP

#include <entt/signal/fwd.hpp>
#include <motor/app/state.hpp>
#include <motor/core/event.hpp>

namespace frontier {

class startup_state : public motor::state {
public:
    startup_state(motor::app& app)
        : motor::state{app},
          dispatcher{app.get_registry().ctx<entt::dispatcher>()} {}

    motor::transition update() override;

protected:
    void on_start() override;
    void on_stop() override;

private:
    entt::dispatcher& dispatcher;
    bool started{};

    void receive_start(const motor::event::start&) { started = true; }
};

} // namespace frontier

#endif // FRONTIER_STARTUP_STATE_HPP
