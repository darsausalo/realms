#include "motor/services/scripts_service.h"
#include "motor/core/prototype_registry.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include "motor/services/mods_service.h"
#include <filesystem>
#include <fstream>

namespace motor {

namespace {

class script_error : public std::runtime_error {
public:
    explicit script_error(std::string_view message)
        : std::runtime_error{std::data(message)} {}
};

// TODO: remove ugly static state
static std::filesystem::path current_mod_path{};

} // namespace

inline int script_panic(lua_State* L) {
    sol::optional<std::string_view> message =
            sol::stack::unqualified_check_get<std::string_view>(L, -1,
                                                                sol::no_panic);
    lua_settop(L, 0);

    throw script_error{message ? message.value() : "unexpected script error"};
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

static void run_mod_scripts(sol::state& lua, std::string_view name,
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
                    locator::files::ref()
                            .get_full_path(fmt::format("mods/core/lib/{}.lua",
                                                       script_name))
                            .string());
        }

        for (auto&& script_name : preloads) {
            lua.script_file(
                    locator::files::ref()
                            .get_full_path(fmt::format("mods/core/lib/{}.lua",
                                                       script_name))
                            .string());
        }

        auto script_filename = fmt::format("{}.lua", name);

        locator::mods::ref().visit([&lua, script_filename](const auto& m) {
            current_mod_path = m.get_path();
            auto path = current_mod_path / script_filename;
            if (std::filesystem::exists(path)) {
                try {
                    lua.script_file(path.string());
                } catch (sol::error& e) {
                    spdlog::error(e.what());
                }
            }
        });
    } catch (script_error& e) {
        spdlog::error(e.what());
    }
}

sol::table scripts_service::load_prototypes(sol::state& prototypes_lua) {
    run_mod_scripts(prototypes_lua, "prototypes", {{"serpent"}},
                    {{"prototypes"}});

    sol::optional<sol::table> maybe_prototypes = prototypes_lua["prototypes"];
    if (maybe_prototypes) {
        auto prototypes = maybe_prototypes.value();
        try {
            sol::function inherits = prototypes["inherits"];
            if (inherits != sol::lua_nil) {
                inherits(prototypes);
            } else {
                spdlog::error("missing inherits function in prototypes");
            }

            sol::optional<sol::table> maybe_defs = prototypes["defs"];
            if (maybe_defs) {
                return maybe_defs.value();
            } else {
                spdlog::error("missing defs in prototypes table");
            }
        } catch (script_error& e) {
            spdlog::error(e.what());
        }
    } else {
        spdlog::error("missing prototypes table");
    }

    return {};
}

} // namespace motor
