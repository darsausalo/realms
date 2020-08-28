#ifndef MOTOR_DARRAY_H
#define MOTOR_DARRAY_H

/* Dynamic array macros */
#define darray(T) T*
#define darr_free(a) ((a) ? mt_free(_mt_daraw(a)), 0 : 0)
#define darr_push(a, v) (_mt_darr_maybegrow(a, 1), (a)[_mt_dan(a)++] = (v))
#define darr_count(a) ((a) ? _mt_dan(a) : 0)
#define darr_add(a, n)                                                         \
    (_mt_darr_maybegrow(a, n), _mt_dan(a) += (n), &(a)[_mt_dan(a) - (n)])
#define darr_last(a) ((a)[_mt_dan(a) - 1])
#define darr_pop(a, idx)                                                       \
    do {                                                                       \
        (a)[idx] = darr_last(a);                                               \
        --_mt_dan(a);                                                          \
    } while (0)
#define darr_pop_last(a)                                                       \
    do { --_mt_dan(a); } while (0)
#define darr_pop_lastn(a, n)                                                   \
    do { _mt_dan(a) -= (n); } while (0)
#define darr_clear(a) ((a) ? (_mt_dan(a) = 0) : 0)
#define darr_reserve(a, n) (darr_add(a, n), darr_clear(a))

/* Dynamic array internal */
#define _mt_daraw(a) (((int*) (a)) - 2)
#define _mt_dam(a) _mt_daraw(a)[0]
#define _mt_dan(a) _mt_daraw(a)[1]

#define _mt_darr_needgrow(a, n) ((a) == 0 || _mt_dan(a) + (n) >= _mt_dam(a))
#define _mt_darr_maybegrow(a, n)                                               \
    (_mt_darr_needgrow(a, (n)) ? _mt_darr_grow(a, n) : 0)
#define _mt_darr_grow(a, n)                                                    \
    (*((void**) &(a)) = _mt_darrgrowf((a), (n), sizeof(*(a))))

static void* _mt_darrgrowf(void* arr, int increment, int itemsize) {
    int  dbl_cur = arr ? 2 * _mt_dam(arr) : 0;
    int  min_needed = darr_count(arr) + increment;
    int  m = dbl_cur > min_needed ? dbl_cur : min_needed;
    int* p = (int*) mt_realloc(arr ? _mt_daraw(arr) : NULL,
                               itemsize * m + mt_sizeof(int) * 2);
    if (p) {
        if (!arr) p[1] = 0;
        p[0] = m;
        return p + 2;
    } else {
        mt_assert(false, MT_OUT_OF_MEMORT, NULL);
        return (void*) (2 * sizeof(int));
    }
}

/* Common arrays */

typedef darray(int) int_array_t;
typedef darray(char*) str_array_t;
typedef darray(sds) sds_array_t;

#endif // MOTOR_DARRAY_H
