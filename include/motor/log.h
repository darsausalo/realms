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
} logLevel_t;

#define log_trace(...) log_log(kLogTrace, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(kLogDebug, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(kLogInfo, __FILE__, __LINE__, __VA_ARGS__)
#define log_warning(...) log_log(kLogWarning, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(kLogError, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(kLogFatal, __FILE__, __LINE__, __VA_ARGS__)

void log_set_level(logLevel_t level);
void log_set_fp(FILE* fp, logLevel_t level);

void log_log(logLevel_t level, const char* file, int line, const char* fmt,
             ...);

#endif // MOTOR_LOG_H
