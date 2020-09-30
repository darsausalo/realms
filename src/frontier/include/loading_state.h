#ifndef FRONTIER_LOADING_STATE_H
#define FRONTIER_LOADING_STATE_H

#include <future>
#include <motor/host/game_state.h>

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
    std::future<void> loading_f;
};

} // namespace frontier

#endif // FRONTIER_LOADING_STATE_H
