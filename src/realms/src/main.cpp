#include "plugins/entry_plugin.hpp"
#include "plugins/game_plugin.hpp"
#include <motor/app/app.hpp>
#include <motor/app/app_builder.hpp>

int main(int argc, char const* argv[]) {
    motor::app::build()
        .add_default_plugins(argc, argv)
        .add_plugin<realms::entry_plugin>()
        .add_plugin<realms::game_plugin>()
        .run();

    return 0;
}
