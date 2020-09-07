#include "motor/host/plugin_context.h"
#include <spdlog/spdlog.h>

namespace motor {

void plugin_context::add_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_adders) {
            f(dispatcher);
        }
    }
}

void plugin_context::remove_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_removers) {
            f(dispatcher);
        }
    }
}

void plugin_context::load_prefabs(prefab_loader& loader) {
    for (auto& m : modules) {
        for (auto& f : m.instance->prefab_loaders) {
            f(loader);
        }
    }
}

void plugin_context::load_snapshot(const entt::snapshot_loader& ss_loadr,
                                   binary_input_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_loaders) {
            f(ss_loadr, ar);
        }
    }
}

void plugin_context::save_snapshot(const entt::snapshot& ss,
                                   binary_output_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_savers) {
            f(ss, ar);
        }
    }
}

void plugin_context::add_module(
        std::string_view module_name,
        std::unique_ptr<system_module>&& module_instance) {
    modules.emplace_back(module_name, std::move(module_instance));
}

std::shared_ptr<spdlog::logger> plugin_context::get_logger() const {
    return spdlog::default_logger();
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <sstream>

namespace motor::test::plugin_context {

enum class id : entt::id_type;

struct position {
    float x;
    float y;
};

struct timer {
    int duration;
    int elapsed;
};

struct health {
    int max;
    int value;
};

struct prefab_tag {};

static const char* json_text = R"({
    "entity1": {
        "motor::test::plugin_context::id": 1,
        "motor::test::plugin_context::position": { "x": 101, "y": 201 },
        "motor::test::plugin_context::timer": { "duration": 1001 },
        "motor::test::plugin_context::health": { "max": 101 },
        "motor::test::plugin_context::prefab_tag": {}
    },
    "entity2": {
        "motor::test::plugin_context::id": 2,
        "motor::test::plugin_context::position": { "x": 102, "y": 202 },
        "motor::test::plugin_context::health": { "max": 102 },
        "entt::tag<\"plugin_context\"_hs>": {}
    },
    "entity3": {
        "motor::test::plugin_context::id": 3,
        "motor::test::plugin_context::position": { "x": 103, "y": 203 },
        "motor::test::plugin_context::timer": { "duration": 1003 }
    }
})";

static nlohmann::json get_j(const nlohmann::json& j,
                            motor::test::plugin_context::id id,
                            std::string_view name) {
    for (auto&& [_, value] : j.items()) {
        if (value["motor::test::plugin_context::id"]
                    .get<motor::test::plugin_context::id>() == id) {
            return value[std::data(name)];
        }
    }
    return {};
};

class system_a : public motor::system {};
class system_b : public motor::system {};
class system_c : public motor::system {};
class system_d : public motor::system {};
class system_e : public motor::system {};

class system_a1 : public motor::system {};
class system_a2 : public motor::system {};

class module_a : public motor::system_module {
public:
    module_a() {
        component<id, position, timer, health>();
        component<prefab_tag, entt::tag<"plugin_context"_hs>>();

        system<system_a1>();

        system<system_e, system_a, system_d>();
        system<system_d, system_b, system_c>();

        system<system_b, system_a>();
        system<system_c, system_b>();
        system<system_a>();

        system<system_a2>();
    }
};

} // namespace motor::test::plugin_context

REFL_AUTO(type(motor::test::plugin_context::id));
REFL_AUTO(type(motor::test::plugin_context::position), field(x), field(y));
REFL_AUTO(type(motor::test::plugin_context::timer), field(duration),
          field(elapsed));
REFL_AUTO(type(motor::test::plugin_context::health), field(max), field(value));
REFL_AUTO(type(motor::test::plugin_context::prefab_tag));
REFL_AUTO(type(entt::tag<"plugin_context"_hs>));

REFL_AUTO(type(motor::test::plugin_context::system_a));
REFL_AUTO(type(motor::test::plugin_context::system_b));
REFL_AUTO(type(motor::test::plugin_context::system_c));
REFL_AUTO(type(motor::test::plugin_context::system_d));
REFL_AUTO(type(motor::test::plugin_context::system_e));
REFL_AUTO(type(motor::test::plugin_context::system_a1));
REFL_AUTO(type(motor::test::plugin_context::system_a2));
REFL_AUTO(type(motor::test::plugin_context::module_a));

TEST_CASE("plugin_context: components") {
    using namespace motor::test::plugin_context;

    entt::registry prefab_reg;
    motor::prefab_loader loader{prefab_reg};

    nlohmann::json j = nlohmann::json::parse(json_text);

    motor::plugin_context ctx;

    ctx.register_module<module_a>();
    ctx.load_prefabs(loader.entities(j));

    CHECK(prefab_reg.view<id, position>().size() == 3);
    CHECK(prefab_reg.view<position>().size() == 3);
    CHECK(prefab_reg.view<health>().size() == 2);
    CHECK(prefab_reg.view<timer>().size() == 2);
    CHECK(prefab_reg.view<prefab_tag>().size() == 1);
    CHECK(prefab_reg.view<entt::tag<"plugin_context"_hs>>().size() == 1);

    std::stringstream data;
    {
        motor::binary_output_archive output{data};
        entt::snapshot ss{prefab_reg};

        ctx.save_snapshot(ss.entities(output), output);
    }

    entt::registry reg;
    {
        motor::binary_input_archive input{data};
        entt::snapshot_loader ss_loader{reg};

        ctx.load_snapshot(ss_loader.entities(input), input);
    }

    CHECK(reg.view<id, position>().size() == 3);
    CHECK(reg.view<position>().size() == 3);
    CHECK(reg.view<health>().size() == 2);
    CHECK(reg.view<timer>().size() == 2);
    CHECK(reg.view<prefab_tag>().size() == 1);
    CHECK(reg.view<entt::tag<"plugin_context"_hs>>().size() == 1);

    reg.view<id, position>().each([&j](const auto& id, const auto& p) {
        auto jv = get_j(j, id, motor::nameof_type<position>());
        CHECK(p.x == jv["x"].get<float>());
        CHECK(p.y == jv["y"].get<float>());
    });
    reg.view<id, timer>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<timer>());
        CHECK(h.duration == jv["duration"].get<int>());
    });
    reg.view<id, health>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<health>());
        CHECK(h.max == jv["max"].get<int>());
    });
}
