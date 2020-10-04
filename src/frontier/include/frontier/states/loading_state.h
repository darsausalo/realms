#ifndef FRONTIER_LOADING_STATE_H
#define FRONTIER_LOADING_STATE_H

#include <motor/host/progress.h>
#include <motor/host/state.h>
#include <thread>

namespace frontier {

class loading_state : public motor::state {
public:
    loading_state(entt::registry& reg);
    ~loading_state() override;

    motor::transition update() override;

private:
    std::thread thread{};
    motor::progress progress{};
};

} // namespace frontier

#endif // FRONTIER_LOADING_STATE_H
