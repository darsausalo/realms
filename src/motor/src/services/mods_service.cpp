#include "motor/services/mods_service.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace motor {

mods_service::mods_service() {
    auto& mods_path = locator::files::ref().get_data_path() / MOTOR_MODS_DIR;

    if (!std::filesystem::exists(mods_path)) {
        spdlog::error("mods not found"); // TODO: exception?
        return;
    }

    for (auto& p : std::filesystem::directory_iterator(mods_path)) {
        mods.push_back(std::make_unique<mod>(p.path().filename().string()));
    }

    // TODO: topology sort mods
}

mods_service::~mods_service() {
    while (!mods.empty()) {
        mods.pop_back();
    }
}

void mods_service::load_plugins() noexcept {
    for (auto&& m : mods) {
        m->load_plugin();
    }
}

void mods_service::start_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m->is_valid()) {
            m->add_systems(dispatcher);
        }
    }
}

void mods_service::stop_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m->is_valid()) {
            m->remove_systems(dispatcher);
        }
    }
}

void mods_service::reload_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m->is_changed()) {
            m->remove_systems(dispatcher);
            m->unload_plugin();

            if (!m->load_plugin()) {
                throw std::runtime_error(fmt::format(
                        "can't update mod '{}' plugin", m->get_name()));
            }

            m->add_systems(dispatcher);
        }
    }
}

} // namespace motor
