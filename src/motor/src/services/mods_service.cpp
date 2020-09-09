#include "motor/services/mods_service.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include <stdexcept>

#include <spdlog/spdlog.h>

namespace motor {

mods_service::mods_service() {
    auto& mods_path = locator::files::ref().get_data_path() / MOTOR_MODS_DIR;

    if (!std::filesystem::exists(mods_path)) {
        throw std::runtime_error(fmt::format("mods directory '{}' not found",
                                             mods_path.string()));
    }


    using mod_desc = std::pair<std::string, std::filesystem::path>;
    std::vector<mod_desc> mod_descs;

    for (auto& p : std::filesystem::directory_iterator(mods_path)) {
        auto path = p.path();
        auto name = path.filename().string();
        path.remove_filename();
        mod_descs.push_back(std::make_pair(name, path));
    }

    mods_path = locator::files::ref().get_user_path() / MOTOR_MODS_DIR;
    if (std::filesystem::exists(mods_path)) {
        for (auto& p : std::filesystem::directory_iterator(mods_path)) {
            auto path = p.path();
            auto name = path.filename().string();
            path.remove_filename();

            auto it = std::find_if(
                    mod_descs.begin(), mod_descs.end(),
                    [&name](auto&& md) { return md.first == name; });
            if (it != mod_descs.end()) {
                it->second = path;
            } else {
                mod_descs.push_back(std::make_pair(name, path));
            }
        }
    }

    // TODO: topology sort mods

    for (auto&& mod_descs : mod_descs) {
        spdlog::debug("mod: name = {}, dir = {}", mod_descs.first,
                      mod_descs.second.string());
        mods.push_back(
                std::make_unique<mod>(mod_descs.first, mod_descs.second));
    }
}

mods_service::~mods_service() {
    while (!mods.empty()) {
        mods.pop_back();
    }
}

void mods_service::load_plugins() {
    for (auto&& m : mods) {
        if (!m->load_plugin()) {
            throw std::runtime_error(
                    fmt::format("can't load mod plugin '{}'", m->get_name()));
        }
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
