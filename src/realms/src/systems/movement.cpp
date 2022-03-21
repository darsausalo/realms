#include "movement.hpp"
#include "components/base.hpp"
#include <motor/app/app_builder.hpp>
#include <motor/core/time.hpp>

namespace realms {

void move(entt::view<entt::get_t<const velocity, position>> view,
          const motor::time& time) {
    for (auto&& [_, v, p] : view.each()) {
        p.x += v.dx * time.delta;
        p.y += v.dy * time.delta;
    }
}

void register_movement(motor::app_builder& app) {
    app.add_system<&move>();
}

} // namespace realms
