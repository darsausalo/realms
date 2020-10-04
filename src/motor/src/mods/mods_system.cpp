#include "mods_system.hpp"
#include "motor/app/locator.hpp"
#include "motor/mods/mods_service.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

mods_system::mods_system(entt::registry& reg) : reg{reg}, watcher{reg} {
    locator::mods::set<mods_service>();
    spdlog::debug("mods_system::start");

    reg.ctx<entt::dispatcher>()
            .sink<event::file_changed>()
            .connect<&mods_system::receive_file_changed>(*this);

    thread = std::thread([this, &reg] {
        prg.update("loading prototypes");

        locator::mods::ref().load_prototypes(reg.ctx<prototype_registry>());

        // TODO: remove --->>>
        auto& protos = reg.ctx<prototype_registry>();
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
            reg.ctx<entt::dispatcher>().trigger<event::start>();

            start_watch_mods();
        }
    } else {
        if (prototypes_changed) {
            prototypes_changed = false;

            locator::mods::ref().load_prototypes(reg.ctx<prototype_registry>());

            reg.ctx<entt::dispatcher>().trigger<event::respawn>();
        }
    }
}

void mods_system::start_watch_mods() {
    locator::mods::ref().visit(
            [this](auto&& m) { watcher.watch_directory(m.get_path()); });
}

void mods_system::receive_file_changed(const event::file_changed& e) {
    spdlog::debug("file changed: {}", e.path.string());
    if (e.path.string().rfind("prototype", 0) == 0) {
        prototypes_changed = true;
    }
}

} // namespace motor
