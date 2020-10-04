#include "motor/core/binary_archive.hpp"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>
#include <fmt/core.h>
#include <iostream>
#include <nameof.hpp>
#include <sstream>

namespace motor::test::binary_archive {

struct weapon {
    float damage;
    float crit;
};

template<typename Archive>
void serialize(Archive& ar, weapon& w) {
    ar.member(M(w.damage));
    ar.member(M(w.crit));
}

struct inventory {
    std::array<weapon, 3> weapons;
};

template<typename Archive>
void serialize(Archive& ar, inventory& inv) {
    ar.member(M(inv.weapons));
}

struct monster {
    int max_health;
    inventory inventory;
};

template<typename Archive>
void serialize(Archive& ar, monster& m) {
    ar.member(M(m.max_health));
    ar.member(M(m.inventory));
}

} // namespace motor::test::binary_archive

TEST_CASE("binary serializization/deserializization: struct") {
    using namespace motor::test::binary_archive;

    std::stringstream s;

    monster monster1 = {100, {{{{0.1f, 0.4f}, {0.2f, 0.5f}, {0.3f, 0.6f}}}}};

    {
        motor::binary_output_archive output{s};
        output(monster1);
    }

    monster monster2;
    {
        motor::binary_input_archive input{s};
        input(monster2);
    }

    CHECK(monster2.max_health == monster1.max_health);
    CHECK(monster2.inventory.weapons[0].damage ==
          monster1.inventory.weapons[0].damage);
    CHECK(monster2.inventory.weapons[1].damage ==
          monster1.inventory.weapons[1].damage);
    CHECK(monster2.inventory.weapons[2].damage ==
          monster1.inventory.weapons[2].damage);
    CHECK(monster2.inventory.weapons[0].crit ==
          monster1.inventory.weapons[0].crit);
    CHECK(monster2.inventory.weapons[1].crit ==
          monster1.inventory.weapons[1].crit);
    CHECK(monster2.inventory.weapons[2].crit ==
          monster1.inventory.weapons[2].crit);
}
