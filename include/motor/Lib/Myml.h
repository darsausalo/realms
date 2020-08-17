/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Myml.h
    Mini-YAML parser.

    Mini YAML represents dictionary tree. Support only 2-space indents.

===============================================================================
*/

#ifndef MOTOR_MYML_H
#define MOTOR_MYML_H

#include <cassert>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace motor::myml {

struct Node;
using NodeMap = std::unordered_map<std::string_view, std::shared_ptr<Node>>;

struct Location {
  std::string_view filename;
  int line;
  int column;

  Location() noexcept : line(-1), column(-1) {}
  Location(const std::string_view& filename, int line, int column) noexcept
      : filename(filename), line(line), column(column) {}
};

struct Node {
  std::string_view value;
  NodeMap content;
  std::string_view comment;

  Location location;

  void merge(const Node& other) noexcept;
  void resolveInherits() noexcept;

  Node& operator[](std::string_view key) {
    assert(content.find(key) != content.end());
    return *content[key];
  }

  bool contains(std::string_view key) {
    return content.find(key) != content.end();
  }
};

std::optional<std::shared_ptr<Node>> parse(std::string_view filename,
                                           std::string_view source);

} // namespace motor::myml

namespace fmt {

template<>
struct [[maybe_unused]] formatter<motor::myml::Location> {
  [[maybe_unused]] static constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}') throw format_error("invalid format");

    return it;
  }

  template<typename FormatContext>
  [[maybe_unused]] auto format(const motor::myml::Location& location,
                               FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}({}, {})", location.filename,
                          location.line, location.column);
  }
};

} // namespace fmt

#endif //MOTOR_MYML_H
