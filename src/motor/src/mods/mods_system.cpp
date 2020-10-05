#include "mods_system.hpp"
#include "motor/app/locator.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/mods/mods_service.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

mods_system::mods_system(entt::registry& registry)
    : registry{registry}, dispatcher{registry.ctx<entt::dispatcher>()},
      prototype_registry{registry.ctx<motor::prototype_registry>()},
      watcher{dispatcher} {
    locator::mods::set<mods_service>();
    spdlog::debug("mods_system::start");

    dispatcher.sink<event::file_changed>()
            .connect<&mods_system::receive_file_changed>(*this);

    thread = std::thread([this] {
        prg.update("loading prototypes");

        locator::mods::ref().load_prototypes(prototype_registry);

        // TODO: remove --->>>
        auto& protos = prototype_registry;
        if (protos.get("soldier"_hs) != entt::null) {
            spdlog::debug("EXISTS!");
        } else {
            auto id = entt::hashed_string::value("soldier");
            spdlog::debug("NOT FOUND: {}({})", "soldier", id);
        }

        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(2.1s);
        // TODO: remove <<<---

        prg.complete();
    });
}

mods_system::~mods_system() {
    if (thread.joinable()) {
        thread.join();
    }

    spdlog::debug("mods_system::stop");
    locator::mods::reset();
}

void mods_system::operator()() {
    if (!loaded) {
        if (prg.is_completed()) {
            loaded = true;
            dispatcher.enqueue<event::start>();

            start_watch_mods();
        }
    }
}

void mods_system::start_watch_mods() {
    locator::mods::ref().visit(
            [this](auto&& m) { watcher.watch_directory(m.get_path()); });
}

void mods_system::receive_file_changed(const event::file_changed& e) {
    spdlog::debug("file changed: {}", e.path.string());
    if (!loaded) {
        spdlog::error("can't hot reload: mods system in loading state");
        return;
    }
    if (e.path.string().rfind("prototype", 0) == 0) {
        locator::mods::ref().load_prototypes(prototype_registry);
        dispatcher.enqueue<event::respawn>();
    }
}

} // namespace motor
