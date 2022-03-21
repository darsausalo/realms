#include "registration.hpp"
#include "hud.hpp"
#include "movement.hpp"
#include "transform.hpp"

namespace realms {

void register_systems(motor::app_builder& app) {
    register_movement(app);
    register_transforms(app);
    register_hud(app);
}

} // namespace realms
