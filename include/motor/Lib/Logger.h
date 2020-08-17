/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Logger.h
    Custom logger.

    Wrapper for spdlog custom logger. Don't use default spdlog logger.

===============================================================================
*/

#ifndef FRONTIER_LOGGER_H
#define FRONTIER_LOGGER_H

#include <spdlog/spdlog.h>

namespace mlog {

template<typename FormatString, typename... Args>
inline void trace(const FormatString& fmt, const Args&... args) {
  logger->trace(fmt, args...);
}

template<typename FormatString, typename... Args>
inline void debug(const FormatString& fmt, const Args&... args) {
  logger->debug(fmt, args...);
}

template<typename FormatString, typename... Args>
inline void info(const FormatString& fmt, const Args&... args) {
  logger->info(fmt, args...);
}

template<typename FormatString, typename... Args>
inline void warn(const FormatString& fmt, const Args&... args) {
  logger->warn(fmt, args...);
}

template<typename FormatString, typename... Args>
inline void error(const FormatString& fmt, const Args&... args) {
  logger->error(fmt, args...);
}

template<typename FormatString, typename... Args>
inline void critical(const FormatString& fmt, const Args&... args) {
  logger->critical(fmt, args...);
}

extern spdlog::logger* logger;

} // namespace log

#endif //FRONTIER_LOGGER_H
