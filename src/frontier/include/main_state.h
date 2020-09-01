#ifndef FRONTIER_MAIN_STATE_H
#define FRONTIER_MAIN_STATE_H

#include <motor/core/game_state.h>

namespace frontier {

class main_state : public motor::game_state {
public:
    main_state() noexcept = default;
    ~main_state() noexcept = default;

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    motor::transition update(entt::registry& reg) override;
};

} // namespace frontier


#endif // FRONTIER_MAIN_STATE_H
