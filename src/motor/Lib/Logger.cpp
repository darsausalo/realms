/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Logger.cpp

===============================================================================
*/

#include "motor/Lib/Logger.h"
#include "motor/Core/Shell.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mlog {

#ifdef MOTOR_HOST

static spdlog::logger* createLogger() {
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(spdlog::level::debug);

  // TODO: use cmake variable to define log filename
  auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/frontier.log", true);
  fileSink->set_level(spdlog::level::trace);

  std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

  return motor::shell->set<spdlog::logger>("default", sinks.begin(),
                                           sinks.end());
}

spdlog::logger* logger = createLogger();

#else
spdlog::logger* logger;
#endif

} // namespace mlog
