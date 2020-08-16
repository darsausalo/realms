/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Myml.cpp
    Mini-YAML parser.

===============================================================================
*/

#include "motor/Lib/Myml.h"
#include <optional>
#include <spdlog/spdlog.h>
#include <stack>

namespace motor::myml {

std::optional<std::shared_ptr<Node>> parse(std::string_view filename,
                                           std::string_view source) {
  int col = 1, line = 1, lineStart = -1, level = 0;
  int spaces = 0, keyStart = -1, keyStop = -1, valueStart = -1, valueStop = -1,
      commentStart = -1;
  auto root = std::make_shared<Node>();
  std::stack<std::shared_ptr<Node>> levels;
  levels.push(root);
  for (int i = 0; i < source.length(); ++i) {
    auto curc = source[i];

    col++;
    if (curc == '\n') {
      Location loc(filename, line, col - 1);

      if (levels.size() <= level) {
        spdlog::error("{}: bad indent", loc);
        return {};
      }

      while (levels.size() > level + 1) levels.pop();

      line++;
      col = 1;

      if (keyStart != -1) {
        if (keyStop == -1) {
          spdlog::error("{}: expected colon", loc);
          return {};
        }

        if (valueStart != -1 && commentStart != -1) {
          valueStop = commentStart - 1;
        }

        if (valueStart != -1 && valueStop == -1) { valueStop = i; }

        std::string_view key, value, comment;
        key = std::string_view(&source[keyStart], keyStop - keyStart);
        if (valueStart != -1 && valueStop != -1) {
          value = std::string_view(&source[valueStart], valueStop - valueStart);
          value.remove_prefix(
              std::min(value.find_first_not_of(" "), value.size()));
          value.remove_suffix(
              std::min(value.find_first_not_of(" "), value.size()));
        }
        if (commentStart != -1) {
          comment =
              std::string_view(&source[commentStart + 1], i - commentStart - 1);
          comment.remove_prefix(
              std::min(comment.find_first_not_of(" "), comment.size()));
        }

        std::shared_ptr<Node> cur = levels.top();
        if (cur->content.find(key) != cur->content.end()) {
          spdlog::error("{}: key '{}' already exists", loc, key);
          return {};
        }
        auto valueNode = std::make_shared<Node>();
        valueNode->value = value;
        valueNode->location = Location(filename, line, level * 2);
        valueNode->comment = comment;
        cur->content.emplace(key, valueNode);
        levels.emplace(valueNode);
      }

      lineStart = i;
      level = 0;
      spaces = 0;
      keyStart = -1;
      keyStop = -1;
      valueStart = -1;
      valueStop = -1;
      commentStart = -1;
    } else if (curc == '\r') {
      lineStart++;
    } else if (curc == ':') {
      if (keyStart != -1) { keyStop = i; }
    } else if (curc <= ' ' && keyStart == -1) {
      if (curc != ' ') {
        spdlog::error("{}: tabs not allowed",
                      Location(filename, line, i - lineStart));
        return {};
      }
      spaces++;
      if (spaces >= 2) {
        spaces = 0;
        level++;
      }
      if (keyStop != -1 && valueStart != -1) { valueStop = i; }
    } else if (curc == '#') {
      commentStart = i;
    } else if (commentStart == -1) {
      if (keyStart == -1) {
        keyStart = i;
      } else if (keyStop != -1 && valueStart == -1) {
        valueStart = i;
      }
    }
  }

  return root;
}

void Node::merge(const Node& other) noexcept {
  if (content.empty() && !other.content.empty()) {
    content = other.content;
    value = std::string_view();
    return;
  }

  if (content.empty() && other.content.empty()) {
    value = other.value;
    return;
  }

  for (auto& it : other.content) {
    auto myIt = content.find(it.first);
    if (myIt == content.end()) {
      content.emplace(it.first, it.second);
    } else {
      myIt->second->merge(*it.second);
    }
  }
}

static void mergeInherits(std::shared_ptr<Node>& node,
                          std::shared_ptr<Node>& parent) {
  if (node->content.empty() && !parent->content.empty()) {
    if (node->value.empty()) { node->content = parent->content; }
    return;
  }

  if (node->content.empty() && parent->content.empty()) {
    if (node->value.empty()) { node->value = parent->value; }
    return;
  }

  for (const auto& it : parent->content) {
    auto myIt = node->content.find(it.first);
    if (myIt == node->content.end()) {
      node->content.emplace(it.first, it.second);
    }
  }
}

static void resolveInheritsFromRoot(std::shared_ptr<Node>& node, Node* root) {
  std::vector<std::shared_ptr<Node>> parents;

  for (auto& it : node->content) {
    if (it.first.find("^inherits") != std::string_view::npos) {
      auto prIt = root->content.find(it.second->value);
      if (prIt != root->content.end()) {
        parents.push_back(prIt->second);
      } else {
        spdlog::error("{}: base node '{}' not found", it.second->location,
                      it.first);
      }
    }
  }

  for (auto& parent : parents) mergeInherits(node, parent);

  std::vector<std::string_view> toRemove;
  for (auto&& it : node->content) {
    if (it.first.find("^inherits") != std::string_view::npos) {
      toRemove.push_back(it.first);
    }
  }
  for (auto key : toRemove) node->content.erase(key);
}

void Node::resolveInherits() noexcept {
  for (auto& it : content) { resolveInheritsFromRoot(it.second, this); }
}

} // namespace motor::myml
