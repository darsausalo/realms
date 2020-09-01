#ifndef FRONTIER_LOADING_STATE_H
#define FRONTIER_LOADING_STATE_H

#include <motor/core/game_state.h>

namespace frontier {

class loading_state : public motor::game_state {
public:
    loading_state() noexcept = default;
    ~loading_state() noexcept = default;

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    motor::transition update(entt::registry& reg) override;

private:
    int progress{};
};

} // namespace frontier

#endif // FRONTIER_LOADING_STATE_H
