//
// Created by denis on 16.08.2020.
//

#include "motor/Lib/Myml.h"
#include <gtest/gtest.h>

TEST(Myml, parse) {
  static const char* src = R"(actor_base: # this is base class
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

  auto result = motor::myml::parse("test", src);

  ASSERT_TRUE(result.has_value());
  // TODO: other asserts
}

TEST(Myml, resolveInherits) {
  static const char* src = R"(actor_base: # this is base class
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

  auto result = motor::myml::parse("test", src);

  ASSERT_TRUE(result.has_value());

  auto root = result.value();

  ASSERT_TRUE(root->content["actor1"]->content.find("tag1") ==
              root->content["actor1"]->content.end());

  result.value()->resolveInherits();

  ASSERT_TRUE(root->content["actor1"]->content.find("tag1") !=
              root->content["actor1"]->content.end());

  ASSERT_TRUE(result.value()->content.find("actor1") !=
              result.value()->content.end());
}
