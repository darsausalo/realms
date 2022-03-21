#include "motor/app/app.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/app/app_state.hpp"
#include "motor/core/progress.hpp"
#include "motor/entity/executor.hpp"
#include "motor/entity/prototype_registry.hpp"
#include <spdlog/spdlog.h>

namespace motor {

using namespace entt::literals;

app::app()
    : dispatcher{registry.set<entt::dispatcher>()}
    , prototypes{registry.set<prototype_registry>()} {
    spdlog::set_level(spdlog::level::debug);
    registry.set<app_state>();
}

app::~app() {
    while (!plugins.empty()) {
        spdlog::debug("removed plugin: {}", plugins.back().name);
        plugins.pop_back();
    }
    if (startup_thread.joinable()) {
        startup_thread.join();
    }
}

app_builder& app::build() {
    static app_builder builder{};
    builder.add_stage("pre_frame"_hs)
        .add_stage("pre_event"_hs)
        .add_stage("event"_hs)
        .add_stage("post_event"_hs)
        .add_stage("pre_update"_hs)
        .add_stage("update"_hs)
        .add_stage("post_update"_hs)
        .add_stage("pre_render"_hs)
        .add_stage("render"_hs)
        .add_stage("post_render"_hs)
        .add_stage("pre_gui"_hs)
        .add_stage("gui"_hs)
        .add_stage("post_gui"_hs)
        .add_stage("post_frame"_hs);

    return builder;
}

void app::run() {
    app_state& state = registry.ctx<app_state>();

    dispatcher.sink<event::quit>().connect<&app::request_quit>(*this);

    bool started{};
    entt::registry startup_registry{};
    startup_registry.set<progress>();

    startup(startup_registry);

    executor executor{scheduler};
    while (!should_quit) {
        if (!started && startup_registry.ctx<progress>().is_completed()) {
            dispatcher.enqueue<event::start>();
            started = true;
            state.state = "entry"_hs;
        }
        executor.run(registry, state.state);
    }
}

void app::startup(entt::registry& startup_registry) {
    startup_thread = std::thread([this, &startup_registry] {
        auto starup_systems = scheduler.startup_graph();
        for (auto&& system : starup_systems) {
            system.callback()(system.data(), startup_registry);
        }
        startup_registry.ctx<progress>().complete();
    });
}

} // namespace motor
