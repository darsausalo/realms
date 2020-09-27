#include "motor/services/scripts_service.h"
#include "motor/services/locator.h"
#include "motor/services/mods_service.h"
#include <filesystem>

namespace motor {

scripts_service::scripts_service() : lua{} {
    lua.open_libraries(sol::lib::base, sol::lib::package);
}

void scripts_service::run_scripts() {
    locator::mods::ref().visit([this](const auto& m) {
        auto path = m.get_path() / "mod.lua";
        if (std::filesystem::exists(path)) {
            auto result = lua.safe_script_file(path.string(),
                                               sol::script_pass_on_error);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error(err.what());
            }
        }
    });
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

TEST_CASE("scripts_service: sol") {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.script("print('Hello, Sol!')");

    auto result = lua.script("bad code", sol::script_pass_on_error);
    CHECK(!result.valid());
}
