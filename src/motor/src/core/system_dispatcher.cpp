#include "motor/core/system_dispatcher.h"
#include "motor/core/type_traits.h"
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
    std::stable_sort(
            std::begin(systems), std::end(systems),
            [](auto&& lhs, auto&& rhs) { return lhs.group < rhs.group; });

    {
        auto it = std::find_if_not(
                std::begin(systems), std::end(systems),
                [](auto&& sd) { return sd.group == system_group::init; });
        if (it != std::end(systems)) {
            std::stable_sort(
                    std::begin(systems), it,
                    [](auto&& lhs, auto&& rhs) { return lhs.host > rhs.host; });
        }
    }
    {
        auto it = std::find_if(
                std::begin(systems), std::end(systems),
                [](auto&& sd) { return sd.group == system_group::present; });
        if (it != std::end(systems)) {
            std::stable_sort(it, std::end(systems), [](auto&& lhs, auto&& rhs) {
                return lhs.host < rhs.host;
            });
        }
    }

    std::vector<std::vector<std::size_t>> all_dependencies;
    for (auto&& sd : systems) {
        all_dependencies.push_back({});
        auto& dependencies = all_dependencies.back();

        std::transform(sd.dependencies.cbegin(), sd.dependencies.cend(),
                       std::back_inserter(dependencies),
                       [=](auto& type_id) -> std::size_t {
                           auto it = std::find_if(
                                   systems.cbegin(), systems.cend(),
                                   [&type_id](auto&& s) {
                                       return s.type_id == type_id;
                                   });
                           return it - systems.cbegin();
                       });
    }
    std::vector<bool> visited;
    visited.resize(all_dependencies.size());
    std::fill(std::begin(visited), std::end(visited), false);

    std::vector<system_desc> sorted;

    std::function<void(std::size_t v)> dfs = [&, this](std::size_t v) {
        visited[v] = true;
        for (auto&& i : all_dependencies[v]) {
            if (!visited[i]) {
                dfs(i);
            }
        }
        sorted.push_back(std::move(this->systems[v]));
    };

    for (auto v = 0u; v < all_dependencies.size(); v++) {
        if (!visited[v]) {
            dfs(v);
        }
    }

    systems = std::move(sorted);
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
            if (it != std::end(systems)) {
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

class system_init_a : public motor::init_system {};
class system_host_init_a : public motor::init_system {};

class system_present_a : public motor::present_system {};
class system_present_b : public motor::present_system {};
class system_host_present_a : public motor::present_system {};

class system_a : public motor::sim_system {};
class system_b : public motor::sim_system {};
class system_c : public motor::sim_system {};
class system_d : public motor::sim_system {};
class system_e : public motor::sim_system {};

class system_a1 : public motor::sim_system {};
class system_a2 : public motor::sim_system {};

} // namespace motor::test::system_dispatcher

namespace motor {

template<>
struct is_host_type<motor::test::system_dispatcher::system_host_init_a>
    : std::true_type {};

template<>
struct is_host_type<motor::test::system_dispatcher::system_host_present_a>
    : std::true_type {};

} // namespace motor

TEST_CASE("system dispatcher: topology sorting") {
    using namespace motor::test::system_dispatcher;

    entt::registry reg;
    motor::system_dispatcher dispatcher{reg};

    dispatcher.add_system<system_host_present_a>();
    dispatcher.add_system<system_present_a>();
    dispatcher.add_system<system_a1>();
    dispatcher.add_system<system_init_a>();
    dispatcher.add_system<system_e, system_a, system_d>();
    dispatcher.add_system<system_d, system_b, system_c>();
    dispatcher.add_system<system_b, system_a>();
    dispatcher.add_system<system_c, system_b>();
    dispatcher.add_system<system_present_b>();
    dispatcher.add_system<system_a>();
    dispatcher.add_system<system_a2>();
    dispatcher.add_system<system_host_init_a>();

    dispatcher.update();

    auto dump = dispatcher.dump();

    CHECK(dump.size() == 12);
    CHECK(dump[0].first ==
          "motor::test::system_dispatcher::system_host_init_a");
    CHECK(dump[1].first == "motor::test::system_dispatcher::system_init_a");
    CHECK(dump[2].first == "motor::test::system_dispatcher::system_a1");
    CHECK(dump[3].first == "motor::test::system_dispatcher::system_a");
    CHECK(dump[4].first == "motor::test::system_dispatcher::system_b");
    CHECK(dump[5].first == "motor::test::system_dispatcher::system_c");
    CHECK(dump[6].first == "motor::test::system_dispatcher::system_d");
    CHECK(dump[7].first == "motor::test::system_dispatcher::system_e");
    CHECK(dump[8].first == "motor::test::system_dispatcher::system_a2");
    CHECK(dump[9].first == "motor::test::system_dispatcher::system_present_a");
    CHECK(dump[10].first == "motor::test::system_dispatcher::system_present_b");
    CHECK(dump[11].first ==
          "motor::test::system_dispatcher::system_host_present_a");
}
