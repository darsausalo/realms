#include "motor/host/mod.h"
#include <spdlog/spdlog.h>

namespace motor {

typedef void (*plugin_entry_func)(plugin_context* ctx);

mod::~mod() {
    unload_plugin();
}

bool mod::is_valid() const noexcept {
    return valid;
}

bool mod::is_changed() const noexcept {
    return valid && dl.is_changed();
}

bool mod::load_plugin() noexcept {
    if (dl.is_loaded()) {
        return true;
    }

    valid = dl.load();
    if (!valid) {
        return false;
    }
    auto entry =
            reinterpret_cast<plugin_entry_func>(dl.get_symbol("plugin_entry"));
    if (!entry) {
        unload_plugin();
        valid = false;
        spdlog::error("plugin_entry not found in '{}'", name);
        return false;
    }

    spdlog::info("mod '{}' loaded", name);

    entry(this);

    return true;
}

void mod::unload_plugin() noexcept {
    system_adders.clear();
    system_removers.clear();
    if (dl.is_loaded()) {
        dl.unload();
        spdlog::info("mod '{}' unloaded", name);
    }
}

void mod::add_systems(system_dispatcher& dispatcher) {
    for (auto& f : system_adders) {
        f(dispatcher);
    }
}

void mod::remove_systems(system_dispatcher& dispatcher) {
    for (auto& f : system_removers) {
        f(dispatcher);
    }
}

std::shared_ptr<spdlog::logger> mod::get_logger() const {
    return spdlog::default_logger();
}

} // namespace motor

//==============================================================================
// TEST

#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>
#include <sstream>

namespace motor::test::mod {

class system_a : public motor::system<motor::system_group::on_update> {};
class system_b : public motor::system<motor::system_group::on_update> {};
class system_c : public motor::system<motor::system_group::on_update> {};
class system_d : public motor::system<motor::system_group::on_update> {};
class system_e : public motor::system<motor::system_group::on_update> {};

class system_a1 : public motor::system<motor::system_group::on_update> {};
class system_a2 : public motor::system<motor::system_group::on_update> {};

class system_b1 : public motor::system<motor::system_group::on_update> {};
class system_b2 : public motor::system<motor::system_group::on_update> {};

} // namespace motor::test::mod

TEST_CASE("mod: systems") {
    using namespace motor::test::mod;

    motor::locator::files::set(MOTOR_TEST_BASE_DIR, MOTOR_TEST_BASE_DIR,
                               MOTOR_TEST_BASE_DIR);

    entt::registry reg;

    motor::mod m{"test", "test", motor::mod_manifest{}};
    m.system<system_a1>();
    m.system<system_e, system_a, system_d>();
    m.system<system_d, system_b, system_c>();
    m.system<system_b, system_a>();
    m.system<system_c, system_b>();
    m.system<system_a>();
    m.system<system_a2>();
    m.system<system_b2, system_b1>();
    m.system<system_b1, system_a1>();

    motor::system_dispatcher dispatcher{reg};

    m.add_systems(dispatcher);
    dispatcher.update();

    auto dump = dispatcher.dump();

    CHECK(dump.size() == 9);
    CHECK(dump[0].first == "class motor::test::mod::system_a1");
    CHECK(dump[1].first == "class motor::test::mod::system_a");
    CHECK(dump[2].first == "class motor::test::mod::system_b");
    CHECK(dump[3].first == "class motor::test::mod::system_c");
    CHECK(dump[4].first == "class motor::test::mod::system_d");
    CHECK(dump[5].first == "class motor::test::mod::system_e");
    CHECK(dump[6].first == "class motor::test::mod::system_a2");
    CHECK(dump[7].first == "class motor::test::mod::system_b1");
    CHECK(dump[8].first == "class motor::test::mod::system_b2");

    m.remove_systems(dispatcher);

    dump = dispatcher.dump();

    CHECK(dump.size() == 0);

    motor::locator::files::reset();
}
