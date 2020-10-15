#include "motor/core/scripts.hpp"
#include "motor/core/filesystem.hpp"
#include <fmt/core.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace {

// TODO: remove ugly static state
static std::filesystem::path current_mod_path{};

} // namespace

inline int script_panic(lua_State* L) {
    sol::optional<std::string_view> message =
        sol::stack::unqualified_check_get<std::string_view>(L, -1,
                                                            sol::no_panic);
    lua_settop(L, 0);

    throw motor::scripts::error{message ? message.value()
                                        : "unexpected script error"};
}

static int load_file_require(lua_State* L) {
    auto package_path = current_mod_path / sol::stack::get<std::string>(L, 1);
    package_path += ".lua";

    if (!std::filesystem::exists(package_path)) {
        sol::stack::push(
            L, fmt::format(" file '{}' is missing", package_path.string()));
        return 1;
    }

    std::string package_data;
    {
        std::ifstream package_file{package_path};
        package_file.seekg(0, std::ios::end);
        package_data.reserve(package_file.tellg());
        package_file.seekg(0, std::ios::beg);

        package_data.assign((std::istreambuf_iterator<char>(package_file)),
                            std::istreambuf_iterator<char>());
    }

    luaL_loadbuffer(L, std::data(package_data), std::size(package_data),
                    std::data(package_path.string()));

    return 1;
}

namespace motor::scripts {

void run(sol::state& lua,
         const std::vector<std::filesystem::path>& script_paths,
         const std::vector<std::string_view>& libs,
         const std::vector<std::string_view>& preloads) {
    lua.set_panic(&script_panic);
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table,
                       sol::lib::math, sol::lib::string);

    try {
        lua.clear_package_loaders();
        lua.add_package_loader(load_file_require);

        for (auto&& script_name : libs) {
            lua.require_file(
                std::string{script_name},
                filesystem::full_path(
                    fmt::format("mods/core/lib/{}.lua", script_name))
                    .string());
        }

        for (auto&& script_name : preloads) {
            lua.script_file(
                filesystem::full_path(
                    fmt::format("mods/core/lib/{}.lua", script_name))
                    .string());
        }

        for (auto&& script_path : script_paths) {
            current_mod_path = script_path.parent_path();
            if (std::filesystem::exists(script_path)) {
                try {
                    lua.script_file(script_path.string());
                } catch (sol::error& e) {
                    spdlog::error(e.what());
                }
            }
        };
    } catch (error& e) {
        spdlog::error(e.what());
    }
}

} // namespace motor::scripts
