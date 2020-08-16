#include "motor/Lib/Myml.h"
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

static const char* tstStr = R"(actor_base: # this is base class
  tag1:
  health:
    max: 100
  speed:
    max: 200

actor1:
  ^inherits: actor_base # inherits from base
  health:
    max: 110
  speed:
    max: 210
  tag2:

actor2:
  ^inherits@1: actor_base # inherits from base
  ^inherits@2: actor1 # inherits from actor1
  health:
    max: 120
  speed:
    max: 220
)";

static const char* tstStr2 = R"(actor_base:
  health:
    max: 1000
  velocity:
    max: 301

actor1:
  tag2:
)";

struct position {
  float x;
  float y;
};

struct velocity {
  float dx;
  float dy;
};

void update(entt::registry& registry) {
  auto view = registry.view<position, velocity>();

  for (auto entity : view) {
    auto& vel = view.get<velocity>(entity);

    vel.dx = 0.;
    vel.dy = 0.;
  }
}

void update(std::uint64_t dt, entt::registry& registry) {
  registry.view<position, velocity>().each([dt](auto& pos, auto& vel) {
    pos.x += vel.dx * dt;
    pos.y += vel.dy * dt;
  });
}

void printNodes(const motor::myml::NodeMap& nodes, int level) {
  std::string indent;
  for (int i = 0; i < level; i++) indent += "  ";
  for (const auto& it : nodes) {
    spdlog::info("{}{}: {} [l: {}, c: {}] # {}", indent, it.first,
                 it.second->value, it.second->location.line,
                 it.second->location.column, it.second->comment);
    printNodes(it.second->content, level + 1);
  }
}

int main(int argc, char* argv[]) {
  entt::registry registry;
  std::uint64_t dt = 16;

  for (auto i = 0; i < 10; ++i) {
    auto entity = registry.create();
    registry.emplace<position>(entity, i * 1.f, i * 1.f);
    if (i % 2 == 0) { registry.emplace<velocity>(entity, i * .1f, i * .1f); }
  }

  update(dt, registry);
  update(registry); // alt update

  registry.view<position, velocity>().each(
      [](entt::entity e, position& pos, velocity& vel) {
        spdlog::info("e = {}; pos = {}, {}; vel = {}, {}", e, pos.x, pos.y,
                     vel.dx, vel.dy);
      });

  auto root = motor::myml::parse("tstStr", tstStr);

  spdlog::info("======= result =======");

  if (root.has_value()) {
    root.value()->resolveInherits();
    printNodes(root.value()->content, 0);
  } else
    spdlog::warn("failed to load");

  std::string_view s0 = "tag1";
  spdlog::info("ff: {}, npos: {}", s0.find_first_of("^inherits"),
               std::string_view::npos);
}
