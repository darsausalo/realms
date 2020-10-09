#include "mods_system.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/events.hpp"
#include "motor/core/filesystem.hpp"
#include "motor/core/scripts.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace motor {

static constexpr char* core_name = "core";

static void sort_mods(std::vector<mod>& mods, std::vector<mod>& broken_mods) {
    std::vector<std::vector<std::size_t>> all_dependencies;

    std::sort(mods.begin(), mods.end(), [](auto&& lhs, auto&& rhs) {
        return lhs.get_name() < rhs.get_name();
    });

    for (auto&& m : mods) {
        all_dependencies.push_back({});
        auto& dependencies = all_dependencies.back();
        auto& mod_dependencies = m.get_manifest().dependencies;
        std::transform(
                std::cbegin(mod_dependencies), std::cend(mod_dependencies),
                std::back_inserter(dependencies),
                [&mods](auto&& name) -> std::size_t {
                    auto it = std::find_if(std::cbegin(mods), std::cend(mods),
                                           [&name](auto&& dm) {
                                               return dm.get_name() == name;
                                           });
                    return it - std::cbegin(mods);
                });
    }

    std::vector<bool> visited;
    visited.resize(all_dependencies.size());
    std::fill(visited.begin(), visited.end(), false);

    auto core_idx =
            std::find_if(std::cbegin(mods), std::cend(mods),
                         [](auto&& m) { return m.get_name() == core_name; }) -
            std::cbegin(mods);

    visited[core_idx] = true;

    std::vector<mod> sorted_mods;
    std::vector<std::size_t> broken;
    std::function<void(std::size_t v)> dfs = [&](std::size_t v) {
        visited[v] = true;
        for (auto&& i : all_dependencies[v]) {
            if (i >= visited.size() ||
                std::find_if(std::cbegin(broken), std::cend(broken),
                             [i](auto& it) { return i == it; }) !=
                        std::cend(broken)) {
                broken.push_back(v);
                return;
            }
            if (!visited[i]) {
                dfs(i);
            }
        }
        sorted_mods.push_back(std::move(mods[v]));
    };
    for (auto v = 0u; v < all_dependencies.size(); v++) {
        if (!visited[v]) {
            dfs(v);
        }
    }

    for (auto i : broken) {
        broken_mods.push_back(std::move(mods[i]));
    }

    sorted_mods.insert(sorted_mods.begin(), std::move(mods[core_idx]));
    mods = std::move(sorted_mods);
}

void from_json(const nlohmann::json& j, mod_manifest& m) {
    if (j.contains("dependencies")) {
        j.at("dependencies").get_to(m.dependencies);
    }
    if (j.contains("author")) {
        j.at("author").get_to(m.author);
    }
    if (j.contains("description")) {
        j.at("description").get_to(m.description);
    }
}

static void load_mods(std::vector<mod>& mods, std::vector<mod>& broken_mods) {
    auto& mods_path = filesystem::data() / "mods";

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

    mods_path = filesystem::user() / "mods";
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

    for (auto&& mod_desc : mod_descs) {
        auto manifest_path = mod_desc.second / mod_desc.first / "manifest.json";
        mod_manifest manifest;
        try {
            std::ifstream manifest_file(manifest_path);
            nlohmann::json manifest_json;
            manifest_file >> manifest_json;

            manifest_json.get_to(manifest);
        } catch (nlohmann::json::exception& e) {
            if (mod_desc.first == core_name) {
                throw std::runtime_error(fmt::format("failed to read '{}': {}",
                                                     manifest_path.string(),
                                                     e.what()));
            } else {
                spdlog::error(fmt::format("failed to read '{}': {}",
                                          manifest_path.string(), e.what()));
                broken_mods.push_back({mod_desc.first, mod_desc.second, {}});
                continue;
            }
        }

        mods.push_back(
                {mod_desc.first, mod_desc.second / mod_desc.first, manifest});
    }

    sort_mods(mods, broken_mods);
}

//==============================================================================

mods_system::mods_system(app_builder& app)
    : registry{app.registry()}, dispatcher{app.dispatcher()},
      prototypes{app.registry().ctx<motor::prototype_registry>()},
      watcher{dispatcher} {
    load_mods(mods, broken_mods);

    dispatcher.sink<event::bootstrap>()
            .connect<&mods_system::receive_bootstrap>(*this);
    dispatcher.sink<event::file_changed>()
            .connect<&mods_system::receive_file_changed>(*this);

    app.add_system_to_stage<&mods_system::update>("pre_frame"_hs, *this);

    dispatcher.enqueue<event::bootstrap>();
}

mods_system::~mods_system() {
    dispatcher.disconnect(*this);

    if (thread.joinable()) {
        thread.join();
    }
}

void mods_system::update() {
    if (!loaded) {
        if (prg.is_completed()) {
            loaded = true;

            dispatcher.enqueue<event::start>();

            start_watch_mods();
        }
    }
}

void mods_system::load_prototypes() {
    std::vector<std::filesystem::path> script_paths;
    for (auto&& m : mods) {
        script_paths.push_back(m.get_path() / "prototypes.lua");
    }

    sol::state lua;
    scripts::run(lua, script_paths, {{"serpent"}}, {{"prototypes"}});

    sol::optional<sol::table> maybe_prototypes = lua["prototypes"];
    if (maybe_prototypes) {
        auto prototypes_table = maybe_prototypes.value();
        try {
            sol::function inherits = prototypes_table["inherits"];
            if (inherits != sol::lua_nil) {
                inherits(prototypes_table);
            } else {
                spdlog::error("missing inherits function in prototypes");
            }

            sol::optional<sol::table> maybe_defs = prototypes_table["defs"];
            if (maybe_defs) {
                prototypes.transpire(maybe_defs.value());
            } else {
                spdlog::error("missing defs in prototypes table");
            }
        } catch (scripts::error& e) {
            spdlog::error(e.what());
        }
    } else {
        spdlog::error("missing prototypes table");
    }
}

void mods_system::start_watch_mods() {
    for (auto&& m : mods) {
        watcher.watch_directory(m.get_path());
    }
}

void mods_system::receive_bootstrap(const event::bootstrap&) {
    thread = std::thread([this] {
        prg.update("loading prototypes");

        load_prototypes();

        prg.complete();
    });
}

void mods_system::receive_file_changed(const event::file_changed& e) {
    spdlog::debug("file changed: {}", e.path.string());
    if (!loaded) {
        spdlog::error("can't hot reload: mods system in loading state");
        return;
    }
    if (e.path.string().rfind("prototype", 0) == 0) {
        load_prototypes();
        prototypes.respawn(registry);
        dispatcher.enqueue<event::respawn>(); // TODO: REMOVE?!!!
    }
}

} // namespace motor
