/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Myml.cpp
    Test: Mini-YAML parser.

===============================================================================
*/

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

  auto& root = *result.value();

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor1"].contains("tag1"));
  ASSERT_EQ("actor_base", root["actor1"]["^inherits"].value);
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor2"].contains("tag1"));
  ASSERT_EQ("actor_base", root["actor2"]["^inherits@1"].value);
  ASSERT_EQ("actor1", root["actor2"]["^inherits@2"].value);
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);
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

  auto& root = *result.value();

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor1"].contains("tag1"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor1"]["^inherits"].value);
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor2"].contains("tag1"));
  ASSERT_FALSE(root["actor2"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor2"]["^inherits@1"].value);
  ASSERT_EQ("actor1", root["actor2"]["^inherits@2"].value);
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);

  root.resolveInherits();

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);

  ASSERT_TRUE(root["actor1"].contains("tag1"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_FALSE(root["actor1"].contains("^inherits"));
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);

  ASSERT_TRUE(root["actor2"].contains("tag1"));
  ASSERT_TRUE(root["actor2"].contains("tag2"));
  ASSERT_FALSE(root["actor2"].contains("^inherits@1"));
  ASSERT_FALSE(root["actor2"].contains("^inherits@2"));
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);
}

TEST(Myml, merge) {
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

  static const char* src1 = R"(actor_base:
  tag3:
  armor:
    max: 15
)";

  auto result = motor::myml::parse("test", src);

  ASSERT_TRUE(result.has_value());

  auto& root = *result.value();

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor1"].contains("tag1"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor1"]["^inherits"].value);
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor2"].contains("tag1"));
  ASSERT_FALSE(root["actor2"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor2"]["^inherits@1"].value);
  ASSERT_EQ("actor1", root["actor2"]["^inherits@2"].value);
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);

  auto result1 = motor::myml::parse("test1", src1);

  ASSERT_TRUE(result1.has_value());

  auto& mod = *result1.value();

  root.merge(mod);

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_TRUE(root["actor_base"].contains("tag3"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);
  ASSERT_EQ("15", root["actor_base"]["armor"]["max"].value);

  ASSERT_FALSE(root["actor1"].contains("tag1"));
  ASSERT_FALSE(root["actor1"].contains("tag3"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor1"]["^inherits"].value);
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);

  ASSERT_FALSE(root["actor2"].contains("tag1"));
  ASSERT_FALSE(root["actor1"].contains("tag3"));
  ASSERT_FALSE(root["actor2"].contains("tag2"));
  ASSERT_EQ("actor_base", root["actor2"]["^inherits@1"].value);
  ASSERT_EQ("actor1", root["actor2"]["^inherits@2"].value);
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);

  root.resolveInherits();

  ASSERT_TRUE(root["actor_base"].contains("tag1"));
  ASSERT_EQ("100", root["actor_base"]["health"]["max"].value);
  ASSERT_EQ("200", root["actor_base"]["speed"]["max"].value);

  ASSERT_TRUE(root["actor1"].contains("tag1"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_TRUE(root["actor1"].contains("tag3"));
  ASSERT_FALSE(root["actor1"].contains("^inherits"));
  ASSERT_EQ("110", root["actor1"]["health"]["max"].value);
  ASSERT_EQ("210", root["actor1"]["speed"]["max"].value);
  ASSERT_EQ("15", root["actor1"]["armor"]["max"].value);

  ASSERT_TRUE(root["actor2"].contains("tag1"));
  ASSERT_TRUE(root["actor2"].contains("tag2"));
  ASSERT_TRUE(root["actor1"].contains("tag2"));
  ASSERT_FALSE(root["actor2"].contains("^inherits@1"));
  ASSERT_FALSE(root["actor2"].contains("^inherits@2"));
  ASSERT_EQ("120", root["actor2"]["health"]["max"].value);
  ASSERT_EQ("220", root["actor2"]["speed"]["max"].value);
  ASSERT_EQ("15", root["actor2"]["armor"]["max"].value);
}
