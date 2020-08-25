/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef MOTOR_LOG_H
#define MOTOR_LOG_H

typedef enum {
    kLogTrace,
    kLogDebug,
    kLogInfo,
    kLogWarning,
    kLogError,
    kLogFatal
} log_level_t;

#define log_trace(...) log_log(kLogTrace, __VA_ARGS__)
#define log_debug(...) log_log(kLogDebug, __VA_ARGS__)
#define log_info(...) log_log(kLogInfo, __VA_ARGS__)
#define log_warning(...) log_log(kLogWarning, __VA_ARGS__)
#define log_error(...) log_log(kLogError, __VA_ARGS__)
#define log_fatal(...) log_log(kLogFatal, __VA_ARGS__)

void log_set_level(log_level_t level);
void log_set_fp(FILE* fp, log_level_t level);

void log_log(log_level_t level, const char* fmt, ...);

#endif // MOTOR_LOG_H
