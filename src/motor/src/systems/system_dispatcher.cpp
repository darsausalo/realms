#include "motor/systems/system_dispatcher.h"
#include <algorithm>
#include <functional>

namespace motor {

system_dispatcher::~system_dispatcher() {
    while (!systems.empty()) {
        systems.back().instance->on_stop(reg);
        systems.pop_back();
    }
}

void system_dispatcher::update() {
    if (need_sorting) {
        sort();
        need_sorting = false;
    }
    for (auto& system : systems) {
        system.instance->update(reg);
    }
}

void system_dispatcher::sort() {
    // TODO: topological sorting
    std::sort(systems.begin(), systems.end(), [](auto& a, auto& b) {
        auto ds = std::find_if(b.dependencies.cbegin(), b.dependencies.cend(),
                               [&a](auto&& b_type_id) {
                                   return a.type_id == b_type_id;
                               }) != b.dependencies.end();
        return ds;
    });
}

std::vector<std::pair<std::string, std::string>> system_dispatcher::dump() {
    std::vector<std::pair<std::string, std::string>> result{};

    for (auto&& sd : systems) {
        std::string name = std::string{sd.name};
        std::string dependencies{};

        for (auto i = 0u; i < sd.dependencies.size(); i++) {
            auto id = sd.dependencies[i];
            auto it = std::find_if(systems.cbegin(), systems.cend(),
                                   [&id](auto&& s) { return s.type_id == id; });
            if (it != systems.end()) {
                dependencies += it->name;
                if (i < sd.dependencies.size() - 1) {
                    dependencies += ", ";
                }
            }
        }

        result.push_back({name, dependencies});
    }
    return std::move(result);
}


} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>
#include <fmt/core.h>
#include <iostream>

namespace motor::test::system_dispatcher {

class system_a : public motor::system {};
class system_b : public motor::system {};
class system_c : public motor::system {};
class system_d : public motor::system {};
class system_e : public motor::system {};

class system_a1 : public motor::system {};
class system_a2 : public motor::system {};

} // namespace motor::test::system_dispatcher

TEST_CASE("testing system_dispatcher sorting") {
    using namespace motor::test::system_dispatcher;

    entt::registry reg;
    motor::system_dispatcher dispatcher{reg};

    dispatcher.add_system<system_a1>();
    dispatcher.add_system<system_e, system_a, system_d>();
    dispatcher.add_system<system_d, system_b, system_c>();
    dispatcher.add_system<system_b, system_a>();
    dispatcher.add_system<system_c, system_b>();
    dispatcher.add_system<system_a>();
    dispatcher.add_system<system_a2>();

    dispatcher.update();

    auto dump = dispatcher.dump();

    CHECK(dump.size() == 7);
    CHECK(dump[0].first == "motor::test::system_dispatcher::system_a1");
    CHECK(dump[1].first == "motor::test::system_dispatcher::system_a");
    CHECK(dump[2].first == "motor::test::system_dispatcher::system_b");
    CHECK(dump[3].first == "motor::test::system_dispatcher::system_c");
    CHECK(dump[4].first == "motor::test::system_dispatcher::system_d");
    CHECK(dump[5].first == "motor::test::system_dispatcher::system_e");
    CHECK(dump[6].first == "motor::test::system_dispatcher::system_a2");
}
