#include "motor/app/app.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/entity/executor.hpp"
#include "motor/entity/prototype_registry.hpp"
#include <spdlog/spdlog.h>

namespace motor {

app::app()
    : dispatcher{registry.set<entt::dispatcher>()},
      prototypes{registry.set<prototype_registry>()} {
    spdlog::set_level(spdlog::level::debug);
}

app::~app() {
    while (!plugins.empty()) {
        spdlog::debug("removed plugin: {}", plugins.back().name);
        plugins.pop_back();
    }
}

app_builder app::build() {
    app_builder builder{};
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
            .add_stage("post_frame"_hs);

    return std::move(builder);
}

void app::run() {
    dispatcher.sink<event::quit>().connect<&app::receive>(*this);

    executor executor{scheduler};
    while (!should_quit) {
        executor.run(registry);
    }
}

} // namespace motor
