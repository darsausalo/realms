#include "registration.hpp"

#include "base.hpp"
#include "base_sery.hpp"
#include <motor/app/app_builder.hpp>

namespace realms {

void register_components(motor::app_builder& app) {
    using namespace entt::literals;
    using spec = motor::component_specifier;

    app.define_component<position, spec::FINAL>()
        .define_component<velocity, spec::FINAL>()
        .define_component<acceleration>()
        .define_component<"player"_hs>();
}

} // namespace realms
