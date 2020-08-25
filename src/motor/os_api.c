#include "private_api.h"

mt_atomic_int_t mt_malloc_count = 0;
mt_atomic_int_t mt_realloc_count = 0;
mt_atomic_int_t mt_calloc_count = 0;
mt_atomic_int_t mt_free_count = 0;

void* mt_malloc(mt_size_t size) {
    mt_assert(size > 0, MT_INVALID_PARAMETER, NULL);
    mt_malloc_count = mt_ainc(&mt_malloc_count);
    return malloc((size_t) size);
}

void mt_free(void* ptr) {
    if (ptr) { mt_free_count = mt_ainc(&mt_free_count); }
    free(ptr);
}

void* mt_realloc(void* ptr, mt_size_t size) {
    mt_assert(size > 0, MT_INVALID_PARAMETER, NULL);
    if (ptr) {
        mt_realloc_count = mt_ainc(&mt_realloc_count);
    } else {
        mt_malloc_count = mt_ainc(&mt_malloc_count);
    }
    return realloc(ptr, (size_t) size);
}

void* mt_calloc(mt_size_t num, mt_size_t size) {
    mt_assert(num > 0 && size > 0, MT_INVALID_PARAMETER, NULL);
    mt_calloc_count = mt_ainc(&mt_calloc_count);
    return calloc((size_t) num, (size_t) size);
}

char* mt_strdup(const char* str) {
    int   len = mt_strlen(str);
    char* result = mt_malloc(len + 1);
    mt_assert(result, MT_OUT_OF_MEMORY, NULL);
    mt_strcpy(result, str);
    return result;
}

char* mt_strndup(const char* str, mt_size_t size) {
    char* result = mt_malloc(size + 1);
    mt_assert(result, MT_OUT_OF_MEMORY, NULL);
    mt_strncpy(result, str, size);
    result[size] = '\0';
    return result;
}
