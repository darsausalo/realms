/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Host.cpp
    Game host.

===============================================================================
*/

#include "motor/Core/Shell.h"
#include "motor/Lib/Logger.h"
#include <cr.h>

static const char* plugin = MOTOR_DEPLOY_DIR "/" CR_PLUGIN("frontier");

int main(int argc, char* argv[]) {
  mlog::info("init plugins: {}", plugin);

  cr_plugin ctx;
  ctx.userdata = motor::shell;
  if (!cr_plugin_open(ctx, plugin)) {
    mlog::error("failed to load: {}", plugin);
    return -1;
  }

  mlog::info("reload plugins");
  cr_plugin_reload(ctx);

  int count = 100;
  while (count-- > 0) {
    mlog::info("update plugins");
    cr_plugin_update(ctx);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
  }

  if (ctx.failure != CR_NONE) {
    mlog::error("FAILURE IN PLUGIN");

    return -1;
  }

  mlog::info("close plugins");
  cr_plugin_close(ctx);

  return 0;
}
