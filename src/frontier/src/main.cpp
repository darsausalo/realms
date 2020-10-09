#include "components/serialization.hpp"
#include "frontier/components/base.hpp"
#include <motor/app/app.hpp>
#include <motor/app/app_builder.hpp>
#include <motor/core/events.hpp>
#include <motor/core/input.hpp>
#include <motor/core/time.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

void quit_system(const motor::input_actions& input,
                 entt::dispatcher& dispatcher) {
    if (input.is_just_pressed("quit"_hs)) {
        dispatcher.trigger<motor::event::quit>();
    }
}

void test_system(
        entt::view<entt::exclude_t<>, motor::timer, position, health> view) {
    view.each([](auto& t, auto& p, auto& h) {
        if (t.finished) {
            t.reset();
            p.x += 1;
            p.y += 1;
            spdlog::debug("p = {},{}", p.x, p.y);
            spdlog::debug("h = {},{}", h.max, h.value);
        }
    });
}

struct test_plugin {
    entt::registry& registry;
    motor::prototype_registry& prototypes;

    test_plugin(motor::app_builder& app)
        : registry{app.registry()}, prototypes{app.prototypes()} {
        app.define_component<position, motor::component_specifier::FINAL>()
                .define_component<velocity>()
                .define_component<health>()
                // .define_component<sprite>()
                .add_system<&quit_system>()
                .add_system<&test_system>();

        app.dispatcher()
                .sink<motor::event::start>()
                .connect<&test_plugin::receive_start>(*this);
    }

    void receive_start(const motor::event::start&) {
        prototypes.spawn(registry, "soldier2"_hs);
    }
};

} // namespace frontier

int main(int argc, char const* argv[]) {
    motor::app::build()
            .add_default_plugins(argc, argv)
            .add_plugin<frontier::test_plugin>()
            .run();

    return 0;
}
