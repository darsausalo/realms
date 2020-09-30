#ifndef FRONTIER_LOADING_STATE_H
#define FRONTIER_LOADING_STATE_H

#include <motor/host/game_state.h>
#include <motor/host/progress.h>
#include <thread>

namespace frontier {

class loading_state : public motor::game_state {
public:
    loading_state() noexcept = default;
    ~loading_state() noexcept = default;

    void on_start(entt::registry& reg, motor::system_dispatcher& disp) override;
    void on_stop(entt::registry& reg, motor::system_dispatcher& disp) override;
    motor::transition update(entt::registry& reg,
                             motor::system_dispatcher& disp) override;

private:
    std::thread thread{};
    motor::progress progress{};
};

} // namespace frontier

#endif // FRONTIER_LOADING_STATE_H
