/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Main.cpp
    Game plugin.

===============================================================================
*/

#include "motor/Core/Logger.h"
#include "motor/Core/Shell.h"
#include <cassert>
#include <cr.h>

CR_EXPORT int cr_main(cr_plugin* ctx, cr_op operation) {
  assert(ctx);

  switch (operation) {
  case CR_LOAD:
    motor::shell = static_cast<motor::Shell*>(ctx->userdata);
    mlog::logger = motor::shell->get<spdlog::logger>();
    mlog::info("plugin loaded");
    return 0;
  case CR_UNLOAD:
    mlog::info("plugin unloaded");
    return 0;
  case CR_CLOSE:
    mlog::info("plugin closed");
    return 0;
  case CR_STEP:
    mlog::info("plugin step");
    return 0;
  }

  return 0;
}
