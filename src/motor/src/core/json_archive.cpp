#include "motor/core/json_archive.h"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>
#include <fmt/core.h>
#include <iostream>
#include <nameof.hpp>
#include <sstream>

namespace motor::test::json_archive {

struct weapon {
    float damage;
    float crit;
};

template<typename Archive>
void serialize(Archive& ar, weapon& w) {
    ar.member(M(w.damage));
    ar.member(M(w.crit));
}

struct monster {
    int max_health;
    weapon weapon;
    std::array<int, 3> inventory;
};

template<typename Archive>
void serialize(Archive& ar, monster& m) {
    ar.member(M(m.max_health));
    ar.member(M(m.weapon));
    ar.member(M(m.inventory));
}

static const char* input_json_str = R"({
    "weapon": {
        "crit": 0.5,
        "damage": 0.1
    },
    "inventory": [1, null, 6],
    "max_health": 100
})";

} // namespace motor::test::json_archive

TEST_CASE("testing json_archive") {
    using namespace motor::test::json_archive;

    nlohmann::json j = nlohmann::json::parse(input_json_str);
    monster monster1;
    {
        motor::json_input_archive input{j};
        serialize(input, monster1);
    }

    CHECK(monster1.max_health == 100);
    CHECK(monster1.weapon.damage == 0.1f);
    CHECK(monster1.weapon.crit == 0.5f);
    CHECK(monster1.inventory[0] == 1);
    CHECK(monster1.inventory[1] == 0);
    CHECK(monster1.inventory[2] == 6);
}
