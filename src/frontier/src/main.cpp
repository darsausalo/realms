#include "components/serialization.hpp"
#include "frontier/components/base.hpp"
#include <motor/app/app.hpp>
#include <motor/app/app_builder.hpp>
#include <motor/core/events.hpp>
#include <motor/core/input.hpp>
#include <motor/core/time.hpp>
#include <motor/entity/map.hpp>
#include <motor/entity/parent.hpp>
#include <motor/entity/transform.hpp>
#include <motor/graphics/camera2d.hpp>
#include <motor/graphics/sprite.hpp>
#include <motor/graphics/sprite_sheet.hpp>
#include <motor/graphics/tile_chunk.hpp>
#include <motor/graphics/tile_set.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>

namespace frontier {

using namespace entt::literals;

void quit_system(const motor::input_actions& input,
                 entt::dispatcher& dispatcher) {
    if (input.is_just_pressed("quit"_hs)) {
        dispatcher.trigger<motor::event::quit>();
    }
}

void test_gui_system() {
    static bool show_demo_window = true;

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
}

void test_system(
    entt::view<entt::get_t<motor::timer, position, const health>> view) {
    view.each([](auto& t, auto& p, auto& h) {
        if (t.finished) {
            t.reset();
            // p.x += 1;
            // p.y += 1;
            spdlog::debug("p = {},{}", p.x, p.y);
            spdlog::debug("h = {},{}", h.max, h.value);
        }
    });
}

void test2_system(
    entt::view<entt::get_t<const motor::timer, const motor::sprite>> view) {
    view.each([](const auto& t, const auto& s) {
        if (t.finished) {
            spdlog::debug("sprite");
        }
    });
}

void update_transforms(
    entt::view<entt::get_t<const position, motor::transform>> view) {
    view.each([](const auto& p, auto& tfm) {
        tfm.value[3] = glm::vec4{p.x, p.y, 0.0f, 1.0f};
    });
}

struct test_plugin {
    entt::registry& registry;
    motor::prototype_registry& prototypes;

    test_plugin(motor::app_builder& app)
        : registry{app.registry()}
        , prototypes{app.prototypes()} {
        app.define_component<position, motor::component_specifier::FINAL>()
            .define_component<velocity>()
            .define_component<health>()
            .define_component<"enemy"_hs>()
            .add_system<&quit_system>()
            .add_system<&update_transforms>()
            .add_system<&test_plugin::update_anim>(*this)
            /*.add_system<&test2_system>()*/
            .add_system<&test_system>()
            .add_system_to_stage<&test_gui_system>("gui"_hs);

        app.dispatcher()
            .sink<motor::event::start>()
            .connect<&test_plugin::receive_start>(*this);

        registry
            .on_construct<position>() //
            .connect<&test_plugin::emplace_transform>();

        registry.ctx<motor::camera2d>().zoom = 2.0f;
        registry.ctx<motor::camera2d>().position = glm::vec2{64.0f};
    }

    static void emplace_transform(entt::registry& registry, entt::entity e) {
        registry.emplace<motor::transform>(e, glm::mat4{1.0f});
    }

    void receive_start(const motor::event::start&) {
        auto e = prototypes.spawn(registry, "soldier2"_hs);
        registry.replace<position>(e, -100.0f, -100.0f);
        std::string info{"spawned soldier2 with:\n"};
        for (auto&& [_, storage] : registry.storage()) {
            if (storage.contains(e)) {
                info += "  ";
                // TODO: maybe remove component_registry?!
                // and just use storage.type().name()?
                info += storage.type().name();
                info += "\n";
            }
        }
        e = prototypes.spawn(registry, "soldier2"_hs);
        registry.replace<position>(e, 100.0f, 100.0f);
        spdlog::debug(info);

        e = prototypes.spawn(registry, "gabe"_hs);
        if (e == entt::null) {
            spdlog::error("failed to spawn gabe");
        } else {
            info = "spawned gabe with:\n";
            for (auto&& [_, storage] : registry.storage()) {
                if (storage.contains(e)) {
                    info += "  ";
                    info += storage.type().name();
                    info += "\n";
                }
            }
            spdlog::debug(info);
        }

        auto map_e = prototypes.spawn(registry, "tstmap1"_hs);
        if (map_e == entt::null) {
            spdlog::error("failed to spawn tstmap");
        } else {
            info = "spawned tstmap with:\n";
            for (auto&& [_, storage] : registry.storage()) {
                if (storage.contains(map_e)) {
                    info += "  ";
                    info += storage.type().name();
                    info += "\n";
                }
            }
            spdlog::debug(info);
            auto map = registry.get<motor::map>(map_e);
            auto tile_set = registry.get<motor::tile_set>(map_e);
            spdlog::debug("map: tile_size={},{}; chunk_size={},{}; tile_set={}",
                          map.tile_size.x, map.tile_size.y, map.chunk_size.x,
                          map.chunk_size.y, tile_set.tiles.size());

            static constexpr const auto N = 4u;
            for (std::size_t i{}; i < N; i++) {
                const auto e = registry.create();
                registry.emplace<motor::parent>(e, map_e);
                auto& tfm =
                    registry.emplace<motor::transform>(e, glm::mat4{1.0f});
                if (i == 3) {
                    tfm.value[3].x = map.chunk_size.x * -map.tile_size.x;
                    // tfm.value[3].y = map.chunk_size.y * -map.tile_size.y;
                } else {
                    tfm.value[3].x = i * 20.0f;
                    tfm.value[3].y = i * -20.0f;
                }
                auto& tile_chunk = registry.emplace<motor::tile_chunk>(e);
                spdlog::debug(
                    "add tile_chunk: e={}; layer={};", entt::to_integral(e), i);
                tile_chunk.layer = i;
                tile_chunk.tiles.resize(map.chunk_size.x * map.chunk_size.y);
                const auto w = map.chunk_size.x;
                const auto h = map.chunk_size.y;
                for (std::size_t x{}; x < map.chunk_size.x; x++) {
                    tile_chunk.tiles[x + (0) * w] = i + 1u;
                    if (i > 0u) {
                        tile_chunk.tiles[x + (h - 1) * w] = i + 1u;
                    }
                }
                /*if (i == 0u || i == 3u)*/ {
                    for (std::size_t y{}; y < map.chunk_size.y; y++) {
                        tile_chunk.tiles[(0) + y * w] = i + 1u;
                        tile_chunk.tiles[(w - 1) + y * w] = i + 1u;
                    }
                }
            }
        }
    }

    motor::timer timer{0.1f};

    void update_anim(entt::view<entt::get_t<motor::sprite_sheet>> view,
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
