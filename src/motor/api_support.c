#include "private_api.h"

void _mt_assert(bool condition, int32_t error_code, const char* param,
                const char* condition_str, const char* file, int line) {
    if (!condition) {
        if (param) {
            log_error("assert(%s) %s:%d: %s (%s)", condition_str, file, line,
                      mt_errstring(error_code), param);
        } else {
            log_error("assert(%s) %s:%d: %s", condition_str, file, line,
                      mt_errstring(error_code));
        }

        host_abort();
    }
}

const char* mt_errstring(int32_t error_code) {
    switch (error_code) {
    case MT_OUT_OF_MEMORY:
        return "out of memory";
    case MT_INVALID_PARAMETER:
        return "invalid parameter";
    case MT_INVALID_HANDLE:
        return "invalid handle";
    }

    return "unknown error code";
}
