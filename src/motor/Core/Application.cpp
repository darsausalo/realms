/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Application.cpp
    Common application loop.

===============================================================================
*/

#include "motor/Core/Application.h"
#include "ShellLocal.h"
#include <SDL.h>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>

namespace motor::app {

namespace fs = std::filesystem;

static void init(int argc, char* argv[]) {
  shell::set<ShellLocal>();

  spdlog::info("initializing");
  static_cast<ShellLocal&>(shell::ref()).init(argc, argv);

  spdlog::info("initialization done");
}

static void shutdown() { static_cast<ShellLocal&>(shell::ref()).shutdown(); }

int run(int argc, char* argv[]) {
  try {
    init(argc, argv);

    spdlog::info("TODO - game loop");

    shutdown();
  } catch (std::exception& e) { spdlog::error(e.what()); }

  return 0;
}

} // namespace motor::app

extern int appMain(int argc, char** argv);

extern "C" [[maybe_unused]] int SDL_main(int argc, char** argv) {
  return appMain(argc, argv);
}
