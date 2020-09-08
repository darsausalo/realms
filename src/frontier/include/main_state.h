#ifndef FRONTIER_MAIN_STATE_H
#define FRONTIER_MAIN_STATE_H

#include <motor/host/game_state.h>

namespace frontier {

class main_state : public motor::game_state {
public:
    main_state() noexcept = default;
    ~main_state() noexcept = default;

    void on_start(entt::registry& reg, motor::system_dispatcher& disp) override;
    void on_stop(entt::registry& reg, motor::system_dispatcher& disp) override;
    motor::transition update(entt::registry& reg,
                             motor::system_dispatcher& disp) override;
};

} // namespace frontier


#endif // FRONTIER_MAIN_STATE_H
