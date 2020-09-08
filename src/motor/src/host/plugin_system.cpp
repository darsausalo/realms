#include "plugin_system.h"
#include "motor/host/mod.h"
#include "motor/platform/dynamic_library.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <entt/entity/registry.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace motor {

typedef void (*plugin_entry_func)(plugin_context* ctx);

plugin_system::~plugin_system() {
}

void plugin_system::on_start(entt::registry& reg) {
    auto& search_path = locator::files::ref().get_data_path() / MOTOR_MODS_DIR;

    search_path.make_preferred();

    if (!std::filesystem::exists(search_path)) {
        spdlog::error("Mods not found");
        return;
    }

    for (auto& p : std::filesystem::directory_iterator(search_path)) {
        libs.push_back(std::make_unique<dynamic_library>(
                p.path().filename().string()));
        auto& dl = *libs.back().get();

        if (!dl.exists()) {
            spdlog::warn("mod dl {} not exists", p.path().filename().string());
            continue;
        }
        if (!dl.load()) {
            spdlog::warn("mod dl {} not loaded", p.path().filename().string());
            continue;
        }

        auto entry = reinterpret_cast<plugin_entry_func>(
                dl.get_symbol("plugin_entry"));
        if (entry) {
            mod ctx;
            entry(&ctx);
        } else {
            spdlog::warn("not found entry");
        }
    }
}

void plugin_system::on_stop(entt::registry& reg) {
    for (auto&& dl : libs) {
        dl->unload();
    }
}

void plugin_system::update(entt::registry& reg) {
    for (auto& dl : libs) {
        if (dl->is_changed()) {
            dl->unload();
            if (dl->load()) {
                auto entry = reinterpret_cast<plugin_entry_func>(
                        dl->get_symbol("plugin_entry"));
                if (entry) {
                    mod ctx;
                    entry(&ctx);
                } else {
                    spdlog::warn("not found entry");
                }
            }
        }
    }
}


} // namespace motor
