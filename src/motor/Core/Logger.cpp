/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Logger.cpp

===============================================================================
*/

#include "motor/Core/Logger.h"
#include "motor/Core/Shell.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mlog {

spdlog::logger* logger;

} // namespace mlog
