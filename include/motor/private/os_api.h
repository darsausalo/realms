#ifndef MOTOR_PRIVATE_OS_API_H
#define MOTOR_PRIVATE_OS_API_H

/* Atomic primitives and operations */
#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#if defined(_MSC_VER)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchangePointer)
#pragma intrinsic(_InterlockedCompareExchangePointer)
#else // _MSC_VER
#include <x86intrin.h>
#endif // _MSC_VER
#endif // _WIN32 || _WIN64

typedef void* volatile mt_atomic_ptr_t;
typedef int volatile mt_atomic_int_t;
typedef int64_t volatile mt_atomic_int64_t;

#if defined(_WIN32) || defined(_WIN64)

#define mt_ainc(a) _InterlockedIncrement((long volatile*) a)
#define mt_adec(a) _InterlockedDecrement((long volatile*) a)
#define mt_axchg(a, b) _InterlockedExchange((long volatile*) a, b)
#define mt_acas(a, xchg, comparand)                                            \
    _InterlockedCompareExchange((long volatile*) a, xchg, comparand)

#define mt_ainc64(a) _InterlockedIncrement(a)
#define mt_adec64(a) _InterlockedDecrement(a)
#define mt_axchg64(a, b) _InterlockedExchange(a, b)
#define mt_acas64(a, xchg, comparand)                                          \
    _InterlockedCompareExchange(a, xchg, comparand)

#define mt_axchg_ptr(a, b) _InterlockedExchangePointer((void* volatile*) a, b)
#define mt_acas_ptr(a, xchg, comparand)                                        \
    _InterlockedCompareExchangePointer((void* volatile*) a, xchg, comparand)

#else

#define mt_ainc(a) __sync_add_and_fetch(a, 1)
#define mt_ainc(a) __sync_sub_and_fetch(a, 1)
#define mt_axchg(a, b) __sync_lock_test_and_set(a, b)
#define mt_acas(a, xchg, comparand)                                            \
    __sync_val_compare_and_swap(a, xchg, comparand)

#define mt_ainc64(a) __sync_add_and_fetch(a, 1)
#define mt_ainc64(a) __sync_sub_and_fetch(a, 1)
#define mt_axchg64(a, b) __sync_lock_test_and_set(a, b)
#define mt_acas64(a, xchg, comparand)                                          \
    __sync_val_compare_and_swap(a, xchg, comparand)

#define mt_axchg_ptr(a, b) __sync_lock_test_and_set(a, b)
#define mt_acas_ptr(a, xchg, comparand)                                        \
    __sync_val_compare_and_swap(a, xchg, comparand)

#endif

/* Memory management */
extern mt_atomic_int_t mt_malloc_count;
extern mt_atomic_int_t mt_realloc_count;
extern mt_atomic_int_t mt_calloc_count;
extern mt_atomic_int_t mt_free_count;

void* mt_malloc(mt_size_t size);
void  mt_free(void* ptr);
void* mt_realloc(void* ptr, mt_size_t size);
void* mt_calloc(mt_size_t num, mt_size_t size);

#if defined(_MSC_VER) || defined(__MINGW32__)
#define mt_alloca(size) _alloca((size_t)(size))
#else // _MSC_VER || __MINGW32__
#define mt_alloca(size) alloca((size_t)(size))
#endif // _MSC_VER || __MINGW32__

/* Strings */
char* mt_strdup(const char* str);
char* mt_strndup(const char* str, mt_size_t size);
#define mt_strlen(str) (mt_size_t) strlen(str)
#define mt_strcmp(str1, str2) strcmp(str1, str2)
#define mt_strncmp(str1, str2, num) strncmp(str1, str2, (size_t)(num))
#define mt_memcmp(ptr1, ptr2, num) memcmp(ptr1, ptr2, (size_t)(num))
#define mt_memcpy(ptr1, ptr2, num) memcpy(ptr1, ptr2, (size_t)(num))
#define mt_memset(ptr, value, num) memset(ptr, value, (size_t)(num))

#if defined(_MSC_VER)
#define mt_strcat(str1, str2) strcat_s(str1, INT_MAX, str2)
#define mt_sprintf(ptr, ...) sprintf_s(ptr, INT_MAX, __VA_ARGS__)
#define mt_vsprintf(ptr, fmt, args) vsprintf_s(ptr, INT_MAX, fmt, args)
#define mt_strcpy(str1, str2) strcpy_s(str1, INT_MAX, str2)
#define mt_strncpy(str1, str2, num)                                            \
    strncpy_s(str1, INT_MAX, str2, (size_t)(num))
#else // _MSC_VER
#define mt_strcat(str1, str2) strcat(str1, str2)
#define mt_sprintf(ptr, ...) sprintf(ptr, __VA_ARGS__)
#define mt_vsprintf(ptr, fmt, args) vsprintf(ptr, fmt, args)
#define mt_strcpy(str1, str2) strcpy(str1, str2)
#define mt_strncpy(str1, str2, num) strncpy(str1, str2, (size_t)(num))
#endif _MSC_VER

/* Path */

#if defined(_WIN32) || defined(_WIN64)
#define MT_OSPATH_SEPARATOR '\\'
#else //defined(_WIN32) || defined(_WIN64)
#define MT_OSPATH_SEPARATOR '/'
#endif // defined(_WIN32) || defined(_WIN64)

#endif // MOTOR_PRIVATE_OS_API_H
