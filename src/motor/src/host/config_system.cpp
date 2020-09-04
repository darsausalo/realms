#include "config_system.h"
#include "../platform/platform.h"
#include "motor/host/storage.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <fstream>
#include <mimalloc.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace motor {

enum class log_level {
    trace = spdlog::level::trace,
    debug = spdlog::level::debug,
    info = spdlog::level::info,
    warn = spdlog::level::warn,
    err = spdlog::level::err,
    critical = spdlog::level::critical,
    off = spdlog::level::off
};

inline void from_json(const nlohmann::json& j, log_level& e) {
    static std::pair<log_level, nlohmann::json> log_levels[] = {
            {log_level::trace, "trace"}, {log_level::debug, "debug"},
            {log_level::info, "info"},   {log_level::warn, "warn"},
            {log_level::err, "err"},     {log_level::critical, "critical"},
            {log_level::off, "off"}};

    auto it = std::find_if(
            std::begin(log_levels), std::end(log_levels),
            [&](const std::pair<log_level, nlohmann::json>& pair) -> bool {
                return pair.second == j;
            });
    e = (it != std::end(log_levels)) ? it->first : log_levels[2].first;
}

static void add_option(nlohmann::json& j, std::string_view key,
                       std::string_view value) {
    auto n = key.find_first_of('.');
    if (n != std::string_view::npos && n) {
        auto k = std::string(key.substr(0, n));
        if (!j.contains(k)) {
            j[k] = {};
        }
        add_option(j[k], key.substr(n + 1), value);
    } else {
        j[std::string(key)] = value;
    }
}

config_system::config_system() {
    base_path = SDL_GetBasePath();
    data_path = base_path / MOTOR_DATA_DIR;
    user_path = SDL_GetPrefPath(MOTOR_PROJECT_ORG, MOTOR_PROJECT_NAME);

    platform::setup_crash_handling(SDL_GetBasePath());
}

config_system::~config_system() {
}

void config_system::on_start(entt::registry& reg) {
    auto& args = reg.ctx<arg_list>();

    nlohmann::json cli_config{};
    std::string key;
    for (auto& arg : args) {
        auto arg_size = arg.size();
        if (arg_size < 3 || arg[0] != '-' || arg[1] != '-') {
            if (!key.empty()) {
                add_option(cli_config, key, arg);
                key.clear();
            }
            continue;
        }
        if (!key.empty()) {
            spdlog::warn("Argument without value: {}", key);
        }
        key = arg.substr(2);
    }

    reg.unset<arg_list>();

    auto& stg = reg.set<storage>(base_path, data_path, user_path);

    if (cli_config.contains("/fs/data_path"_json_pointer)) {
        stg.set_data_path(
                cli_config["/fs/data_path"_json_pointer].get<std::string>());
    }

    if (cli_config.contains("/fs/user_path"_json_pointer)) {
        stg.set_user_path(
                cli_config["/fs/user_path"_json_pointer].get<std::string>());
    }

    auto user_path = stg.get_user_path();

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error(
                fmt::format("SDL initialization failed: {}", SDL_GetError()));
    }

    spdlog::info("{} v{} started", MOTOR_PROJECT_TITLE, MOTOR_PROJECT_VERSION);
    spdlog::info("mimalloc: {}", mi_version());

    spdlog::info("base path: {}", stg.get_base_path().string());
    spdlog::info("data path: {}", stg.get_data_path().string());
    spdlog::info("user path: {}", stg.get_user_path().string());

    auto& config = reg.set<core_config>();

    std::ifstream cfg_file(stg.get_full_path("config.json"));
    try {
        cfg_file >> config;
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("failed to read config.json: {}", e.what());
    }

    config.merge_patch(cli_config);

    if (config.contains("/logging/level"_json_pointer)) {
        log_level ll;
        config["/logging/level"_json_pointer].get_to(ll);
        spdlog::set_level(static_cast<spdlog::level::level_enum>(ll));
    }

    reg.ctx<entt::dispatcher>()
            .sink<event::config_changed>()
            .connect<&config_system::receive_config_changed>(*this);

    spdlog::debug("config_system::started");
}

void config_system::on_stop(entt::registry& reg) {
    SDL_Quit();
    spdlog::debug("config_system::stopped");
}

void config_system::update(entt::registry& reg) {
    if (modified) {
        try {
            auto& stg = reg.ctx<storage>();
            std::ofstream cfg_file(stg.get_full_path("config.json", true));
            cfg_file << std::setw(4) << reg.ctx<core_config>();
        } catch (std::exception& e) {
            spdlog::error("failed to write config.json save: {}", e.what());
        }

        modified = false;
    }
}

void config_system::receive_config_changed(const event::config_changed&) {
    modified = true;
}

} // namespace motor
