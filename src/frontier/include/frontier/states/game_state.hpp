#ifndef FRONTIER_GAME_STATE_HPP
#define FRONTIER_GAME_STATE_HPP

#include <motor/app/state.hpp>
#include <motor/core/event.hpp>

namespace frontier {

class game_state : public motor::state {
public:
    game_state(entt::registry& reg);
    ~game_state();

    motor::transition update() override;

private:
    entt::registry world_reg;

    void receive_respawn(const motor::event::respawn&);
};

} // namespace frontier


#endif // FRONTIER_GAME_STATE_HPP
