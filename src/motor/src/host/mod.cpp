#include "motor/host/mod.h"
#include <spdlog/spdlog.h>

namespace motor {

void mod::add_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_adders) {
            f(dispatcher);
        }
    }
}

void mod::remove_systems(system_dispatcher& dispatcher) {
    for (auto& m : modules) {
        for (auto& f : m.instance->system_removers) {
            f(dispatcher);
        }
    }
}

void mod::load_prefabs(prefab_loader& loader) {
    for (auto& m : modules) {
        for (auto& f : m.instance->prefab_loaders) {
            f(loader);
        }
    }
}

void mod::load_snapshot(const entt::snapshot_loader& ss_loadr,
                        binary_input_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_loaders) {
            f(ss_loadr, ar);
        }
    }
}

void mod::save_snapshot(const entt::snapshot& ss, binary_output_archive& ar) {
    for (auto& m : modules) {
        for (auto& f : m.instance->snapshot_savers) {
            f(ss, ar);
        }
    }
}

void mod::add_module(std::string_view module_name,
                     std::unique_ptr<system_module>&& module_instance) {
    modules.emplace_back(module_name, std::move(module_instance));
}

std::shared_ptr<spdlog::logger> mod::get_logger() const {
    return spdlog::default_logger();
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <sstream>

namespace motor::test::mod {

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
        "motor::test::mod::id": 1,
        "motor::test::mod::position": { "x": 101, "y": 201 },
        "motor::test::mod::timer": { "duration": 1001 },
        "motor::test::mod::health": { "max": 101 },
        "motor::test::mod::prefab_tag": {}
    },
    "entity2": {
        "motor::test::mod::id": 2,
        "motor::test::mod::position": { "x": 102, "y": 202 },
        "motor::test::mod::health": { "max": 102 },
        "entt::tag<\"mod\"_hs>": {}
    },
    "entity3": {
        "motor::test::mod::id": 3,
        "motor::test::mod::position": { "x": 103, "y": 203 },
        "motor::test::mod::timer": { "duration": 1003 }
    }
})";

static nlohmann::json get_j(const nlohmann::json& j, motor::test::mod::id id,
                            std::string_view name) {
    for (auto&& [_, value] : j.items()) {
        if (value["motor::test::mod::id"].get<motor::test::mod::id>() == id) {
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
        component<prefab_tag, entt::tag<"mod"_hs>>();

        system<system_a1>();

        system<system_e, system_a, system_d>();
        system<system_d, system_b, system_c>();

        system<system_b, system_a>();
        system<system_c, system_b>();
        system<system_a>();

        system<system_a2>();
    }
};

} // namespace motor::test::mod

REFL_AUTO(type(motor::test::mod::id));
REFL_AUTO(type(motor::test::mod::position), field(x), field(y));
REFL_AUTO(type(motor::test::mod::timer), field(duration), field(elapsed));
REFL_AUTO(type(motor::test::mod::health), field(max), field(value));
REFL_AUTO(type(motor::test::mod::prefab_tag));
REFL_AUTO(type(entt::tag<"mod"_hs>));

REFL_AUTO(type(motor::test::mod::system_a));
REFL_AUTO(type(motor::test::mod::system_b));
REFL_AUTO(type(motor::test::mod::system_c));
REFL_AUTO(type(motor::test::mod::system_d));
REFL_AUTO(type(motor::test::mod::system_e));
REFL_AUTO(type(motor::test::mod::system_a1));
REFL_AUTO(type(motor::test::mod::system_a2));
REFL_AUTO(type(motor::test::mod::module_a));

TEST_CASE("mod: components") {
    using namespace motor::test::mod;

    entt::registry prefab_reg;
    motor::prefab_loader loader{prefab_reg};

    nlohmann::json j = nlohmann::json::parse(json_text);

    motor::mod ctx;

    ctx.register_module<module_a>();
    ctx.load_prefabs(loader.entities(j));

    CHECK(prefab_reg.view<id, position>().size() == 3);
    CHECK(prefab_reg.view<position>().size() == 3);
    CHECK(prefab_reg.view<health>().size() == 2);
    CHECK(prefab_reg.view<timer>().size() == 2);
    CHECK(prefab_reg.view<prefab_tag>().size() == 1);
    CHECK(prefab_reg.view<entt::tag<"mod"_hs>>().size() == 1);

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
    CHECK(reg.view<entt::tag<"mod"_hs>>().size() == 1);

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
