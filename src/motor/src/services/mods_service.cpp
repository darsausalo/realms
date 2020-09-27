#include "motor/services/mods_service.h"
#include "motor/core/json_archive.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>

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

mods_service::~mods_service() {
    while (!mods.empty()) {
        mods.pop_back();
    }
}

void mods_service::load_plugins() {
    for (auto&& m : mods) {
        if (!m.load_plugin()) {
            throw std::runtime_error(
                    fmt::format("can't load mod plugin '{}'", m.get_name()));
        }
    }
}

void mods_service::start_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m.is_valid()) {
            m.add_systems(dispatcher);
        }
    }
}

void mods_service::stop_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m.is_valid()) {
            m.remove_systems(dispatcher);
        }
    }
}

void mods_service::reload_plugins(system_dispatcher& dispatcher) {
    for (auto&& m : mods) {
        if (m.is_changed()) {
            m.remove_systems(dispatcher);
            m.unload_plugin();

            if (!m.load_plugin()) {
                throw std::runtime_error(fmt::format(
                        "can't update mod '{}' plugin", m.get_name()));
            }

            m.add_systems(dispatcher);
        }
    }
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

TEST_CASE("mods_service: sorting") {
    std::vector<motor::mod> mods;
    std::vector<motor::mod> broken_mods;

    mods.push_back({"a", "a", {{"core"}}});
    mods.push_back({"b", "b", {{"a", "f"}}});
    mods.push_back({"e", "e", {{"c", "d"}}});
    mods.push_back({"c", "c", {{"a", "b"}}});
    mods.push_back({"core", "core", {}});
    mods.push_back({"d", "d", {{"c"}}});
    mods.push_back({"bad2", "bad2", {{"bad"}}});
    mods.push_back({"bad", "bad", {{"unknwon"}}});
    mods.push_back({"f", "f", {{"core"}}});

    motor::sort_mods(mods, broken_mods);

    CHECK(mods[0].get_name() == motor::core_name);
    CHECK(mods[1].get_name() == "a");
    CHECK(mods[2].get_name() == "f");
    CHECK(mods[3].get_name() == "b");
    CHECK(mods[4].get_name() == "c");
    CHECK(mods[5].get_name() == "d");
    CHECK(mods[6].get_name() == "e");

    CHECK(broken_mods[0].get_name() == "bad");
    CHECK(broken_mods[1].get_name() == "bad2");
}
