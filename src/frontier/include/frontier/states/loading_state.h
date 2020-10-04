#ifndef FRONTIER_LOADING_STATE_H
#define FRONTIER_LOADING_STATE_H

#include <motor/core/event.hpp>
#include <motor/host/state.h>

namespace frontier {

class loading_state : public motor::state {
public:
    loading_state(entt::registry& reg);

    motor::transition update() override;

private:
    bool started{};

    void receive_start(const motor::event::start&) { started = true; }
};

} // namespace frontier

#endif // FRONTIER_LOADING_STATE_H
