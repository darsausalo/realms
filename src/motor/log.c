/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "motor.h"
#include <time.h>

static struct {
    log_level_t level;

    FILE*       fp;
    log_level_t file_level;
} logger = {.fp = NULL, .level = kLogInfo};

static const char* level_strings[] = {"TRACE", "DEBUG", "INFO",
                                      "WARN",  "ERROR", "FATAL"};

static const char* level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m",
                                     "\x1b[33m", "\x1b[31m", "\x1b[35m"};

void log_set_level(log_level_t level) {
    // TODO: lock()

    logger.level = level;

    // TODO: unlock()
}
void log_set_fp(FILE* fp, log_level_t level) {
    // TODO: lock()

    logger.fp = fp;
    logger.file_level = level;

    // TODO: unlock()
}

void log_log(log_level_t level, const char* fmt, ...) {
    // TODO: lock()

    if (level >= logger.level) {
        time_t     t = time(NULL);
        struct tm* local_time = localtime(&t);

        // log to stderr
        {
            va_list ap;
            va_start(ap, fmt);

            char buf[16];
            buf[strftime(buf, sizeof(buf), "%H:%M:%S", local_time)] = '\0';
            fprintf(stderr, "[ %s %s%-5s\x1b[0m\x1b[0m] ", buf,
                    level_colors[level], level_strings[level]);
            vfprintf(stderr, fmt, ap);
            fprintf(stderr, "\n");

            va_end(ap);
        }

        // log to file
        if (logger.fp && level >= logger.file_level) {
            va_list ap;
            va_start(ap, fmt);

            char buf[64];
            buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local_time)] =
                '\0';
            fprintf(logger.fp, "[ %s %-5s ]", buf, level_strings[level]);
            vfprintf(logger.fp, fmt, ap);
            fprintf(logger.fp, "\n");
            fflush(logger.fp);

            va_end(ap);
        }
    }

    // TODO: unlock()
}
