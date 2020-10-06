#ifndef FRONTIER_GAME_STATE_HPP
#define FRONTIER_GAME_STATE_HPP

#include <entt/signal/fwd.hpp>
#include <motor/app/state.hpp>
#include <motor/core/events.hpp>
#include <motor/entity/prototype_registry.hpp>

namespace frontier {

class game_state : public motor::state {
public:
    game_state(motor::app& app)
        : motor::state{app}, registry{app.get_registry()},
          dispatcher{registry.ctx<entt::dispatcher>()},
          prototype_registry{registry.ctx<motor::prototype_registry>()} {}

    motor::transition update() override;

protected:
    void on_start() override;
    void on_stop() override;

private:
    entt::registry& registry;
    motor::prototype_registry& prototype_registry;
    entt::dispatcher& dispatcher;

    void receive_respawn(const motor::event::respawn&);
};

} // namespace frontier


#endif // FRONTIER_GAME_STATE_HPP
