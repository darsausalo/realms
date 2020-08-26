#ifndef MOTOR_API_DEFINES_H
#define MOTOR_API_DEFINES_H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* API support defines */

typedef uint32_t mt_flags32_t;
typedef uint32_t mt_flags64_t;

typedef int mt_size_t;
#define mt_sizeof(T) ((mt_size_t) sizeof(T))

#define mt_min(a, b) (((a) < (b)) ? (a) : (b))
#define mt_max(a, b) (((a) > (b)) ? (a) : (b))

/* Debug and support */

void mt_assert(bool condition, int32_t error_code, const char* param,
               const char* condition_str, const char* file, int line);

#ifndef NDEBUG
#define mt_assert(condition, error_code, param)
#else
#define mt_assert(condition, error_code, param)                                \
    _mt_assert(condition, error_code, param, #condition, __FILE__, __LINE__);  \
    assert(condition)
#endif // NDEBUG

/* Errors management */

#define MT_OUT_OF_MEMORY (1)
#define MT_INVALID_PARAMETER (2)
#define MT_INVALID_HANDLE (3)

const char* mt_errstring(int32_t error_code);

#endif // MOTOR_API_DEFINES_H
