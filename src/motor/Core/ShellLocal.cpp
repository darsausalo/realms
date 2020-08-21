/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Shell.cpp
    Game host shell - main context to operate between host and user-space.

===============================================================================
*/

#include "ShellLocal.h"
#include <SDL.h>
#include <cxxopts.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <toml++/toml.h>

namespace motor {

namespace fs = std::filesystem;

void ShellLocal::init(int argc, char** argv) {
  cxxopts::Options options(MOTOR_APP_NAME, "todo add description");

#ifdef MOTOR_NOINSTALL
  std::string defaultBaseDir = MOTOR_DEFAULT_BASE_DIR;
  std::string defaultUserDir = MOTOR_DEFAULT_USER_DIR;
#else
  std::string defaultBaseDir = (fs::path(SDL_GetBasePath()) / "data").string();
  std::string defaultUserDir = SDL_GetPrefPath(MOTOR_ORG_NAME, MOTOR_APP_NAME);
#endif
  baseDir = defaultBaseDir;
  userDir = defaultUserDir;

  try {
    options.add_options()(
        "baseDir", "base directory",
        cxxopts::value<std::string>()->default_value(defaultBaseDir))(
        "userDir", "user directory",
        cxxopts::value<std::string>()->default_value(defaultUserDir));

    // TODO: add other from toml config

    auto result = options.parse(argc, argv);

    baseDir = result["baseDir"].as<std::string>();
    userDir = result["userDir"].as<std::string>();
  } catch (cxxopts::OptionException& e) { spdlog::error(e.what()); }

  spdlog::info("init shell");
  spdlog::info("baseDir: {}", baseDir.generic_string());
  spdlog::info("userDir: {}", userDir.generic_string());

  // TODO: read config
  toml::table config;
  try {
    config = toml::parse_file((baseDir / "config.toml").string());
    for (auto& [k, v] : config) {
      v.visit([k](auto&& n) {
        spdlog::info("k={}", k);
        if constexpr (toml::is_string<decltype(n)>) {
          spdlog::info("v(str)={}", n.value_or(""));
        } else if constexpr (toml::is_boolean<decltype(n)>) {
          spdlog::info("v(bool)={}", n.value_or(false));
        } else if constexpr (toml::is_integer<decltype(n)>) {
          spdlog::info("v(int)={}", n.value_or<int>(0));
        } else if constexpr (toml::is_table<decltype(n)>) {
          for (auto& [k, v] : n) {
            spdlog::info("k2={}", k);
            v.visit([k](auto&& n) {
              if constexpr (toml::is_string<decltype(n)>) {
                spdlog::info(" v(str)={}", n.value_or(""));
              } else if constexpr (toml::is_boolean<decltype(n)>) {
                spdlog::info(" v(bool)={}", n.value_or(false));
              } else if constexpr (toml::is_integer<decltype(n)>) {
                spdlog::info(" v(int)={}", n.value_or<int>(0));
              }
            });
          }
        }
      });
    }

    // auto fullscreen = config["window"]["fullscreen"].as_boolean();
    // auto monitor = config["window"]["monitor"].
    // spdlog::info("window.fullscreen: {}, monitor: {}",
    //              ,
    //              config["window"]["monitor"].as_string());
  } catch (const toml::parse_error& e) {
    std::string sourcePath = !e.source().path ? *e.source().path : "unknown";
    spdlog::error("{}({}, {}): {}", sourcePath, e.source().begin.line,
                  e.source().begin.column, e.description());
  }

  spdlog::info("baseDir: {}", baseDir.generic_string());
  spdlog::info("userDir: {}", userDir.generic_string());
}

void ShellLocal::shutdown() {}

std::ifstream ShellLocal::openReadText(std::string_view filename) {
  return std::ifstream();
}

std::ifstream ShellLocal::openReadBinary(std::string_view filename) {
  return std::ifstream();
}

std::ofstream ShellLocal::openWriteText(std::string_view filename) {
  return std::ofstream();
}

std::ofstream ShellLocal::openWriteBinary(std::string_view filename) {
  return std::ofstream();
}

} // namespace motor
