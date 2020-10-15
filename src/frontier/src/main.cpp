#include "components/serialization.hpp"
#include "frontier/components/base.hpp"
#include <motor/app/app.hpp>
#include <motor/app/app_builder.hpp>
#include <motor/core/events.hpp>
#include <motor/core/input.hpp>
#include <motor/core/time.hpp>
#include <motor/entity/transform.hpp>
#include <motor/graphics/sprite.hpp>
#include <motor/graphics/sprite_sheet.hpp>
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

void test2_system(
    entt::view<entt::exclude_t<>, const motor::timer, const motor::sprite>
        view) {
    view.each([](const auto& t, const auto& s) {
        if (t.finished) {
            spdlog::debug("sprite");
        }
    });
}

void update_transforms(
    entt::view<entt::exclude_t<>, const position, motor::transform> view) {
    view.each([](const auto& p, auto& tfm) {
        tfm.value[3] = glm::vec4{p.x, p.y, 0.0f, 1.0f};
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
            .add_system<&quit_system>()
            .add_system<&update_transforms>()
            .add_system<&test_plugin::update_anim>(*this)
            /*.add_system<&test2_system>()
            .add_system<&test_system>()*/
            ;

        app.dispatcher()
            .sink<motor::event::start>()
            .connect<&test_plugin::receive_start>(*this);

        registry
            .on_construct<position>() //
            .connect<&test_plugin::emplace_transform>(*this);
    }

    void emplace_transform(entt::registry& registry, entt::entity e) {
        registry.emplace<motor::transform>(e, glm::mat4{1.0f});
    }

    void receive_start(const motor::event::start&) {
        auto e = prototypes.spawn(registry, "soldier2"_hs);
        registry.replace<position>(e, -100.0f, -100.0f);
        std::string info{"spawned soldier2 with:\n"};
        registry.visit(e, [&info](auto&& type_info) {
            info += "  ";
            info += type_info.name();
            info += "\n";
        });
        e = prototypes.spawn(registry, "soldier2"_hs);
        registry.replace<position>(e, 100.0f, 100.0f);
        spdlog::debug(info);

        e = prototypes.spawn(registry, "gabe"_hs);
        if (e == entt::null) {
            spdlog::error("failed to spawn gabe");
        } else {
            info = "spawned gabe with:\n";
            registry.visit(e, [&info](auto&& type_info) {
                info += "  ";
                info += type_info.name();
                info += "\n";
            });
            spdlog::debug(info);
        }
    }

    motor::timer timer{0.1f};

    void update_anim(entt::view<entt::exclude_t<>, motor::sprite_sheet> view,
                     const motor::time& time) {
        timer.tick(time.delta);
        if (timer.finished) {
            timer.reset();
            view.each([](auto& s) { s.index++; });
        }
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
