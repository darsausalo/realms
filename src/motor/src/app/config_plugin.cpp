#include "config_plugin.hpp"
#include "core/internal_filesystem.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/filesystem.hpp"
#include "platform/platform.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <filesystem>
#include <fstream>
#include <mimalloc.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
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
        {log_level::trace, "trace"},       //
        {log_level::debug, "debug"},       //
        {log_level::info, "info"},         //
        {log_level::warn, "warn"},         //
        {log_level::err, "err"},           //
        {log_level::critical, "critical"}, //
        {log_level::off, "off"}            //
    };

    auto it = std::find_if(
        std::begin(log_levels), std::end(log_levels),
        [&](const std::pair<log_level, nlohmann::json>& pair) -> bool {
            return pair.second == j;
        });
    e = (it != std::end(log_levels)) ? it->first : log_levels[2].first;
}

static void add_option(nlohmann::json& j,
                       std::string_view key,
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

config_plugin::config_plugin(const arg_list& args, app_builder& app)
    : config{app.registry().ctx_or_set<nlohmann::json>()} {
    platform::setup_crash_handling(SDL_GetBasePath());

    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    spdlog::sinks_init_list sinks_list({stdout_sink});
#if defined(_WIN32)
    auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_st>();
    sinks_list = spdlog::sinks_init_list({stdout_sink, msvc_sink});
#endif // defined(_WIN32)

    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>(MOTOR_PROJECT_NAME, sinks_list));

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

    std::filesystem::path base_path = SDL_GetBasePath();
    std::filesystem::path data_path = base_path / MOTOR_DATA_DIR;
    std::filesystem::path user_path =
        SDL_GetPrefPath(MOTOR_PROJECT_ORG, MOTOR_PROJECT_NAME);

    if (cli_config.contains("/fs/data_path"_json_pointer)) {
        data_path = cli_config["/fs/data_path"_json_pointer].get<std::string>();
    }

    if (cli_config.contains("/fs/user_path"_json_pointer)) {
        user_path = cli_config["/fs/user_path"_json_pointer].get<std::string>();
    }

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error(
            fmt::format("SDL initialization failed: {}", SDL_GetError()));
    }

    spdlog::info("{} v{} started", MOTOR_PROJECT_TITLE, MOTOR_PROJECT_VERSION);
    spdlog::info("mimalloc: {}", mi_version());

    filesystem::init(base_path, data_path, user_path);

    spdlog::info("base path: {}", filesystem::base().string());
    spdlog::info("data path: {}", filesystem::data().string());
    spdlog::info("user path: {}", filesystem::user().string());

    std::ifstream cfg_file(filesystem::full_path("config.json"));
    try {
        cfg_file >> config;
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("failed to read config.json: {}", e.what());
    }
    if (cli_config.is_object() && !cli_config.is_null()) {
        config.merge_patch(cli_config);
    }

    if (config.contains("/logging/level"_json_pointer)) {
        log_level ll;
        config["/logging/level"_json_pointer].get_to(ll);
        spdlog::set_level(static_cast<spdlog::level::level_enum>(ll));
    }

    app.dispatcher()
        .sink<event::config_changed>()
        .connect<&config_plugin::receive>(*this);
}

config_plugin::~config_plugin() { SDL_Quit(); }

void config_plugin::receive(const event::config_changed&) {
    try {
        std::ofstream cfg_file(filesystem::full_path("config.json", true));
        cfg_file << std::setw(4) << config;
    } catch (std::exception& e) {
        spdlog::error("failed to write config.json save: {}", e.what());
    }
}

} // namespace motor
