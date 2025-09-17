/**
 * temp_array.h - Temporary Associative Arrays for C
 *
 * Provides macros for declaring and using temporary associative arrays
 * that exist only for the duration of a function call. Arrays are
 * automatically created and destroyed with each function invocation.
 * Includes robust error handling while maintaining GNU C style.
 */

#ifndef TEMP_ARRAY_H
#define TEMP_ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Simple hash function for parameter combination */
#define TEMP_HASH(a, b, c, d) \
    ((((size_t)(a) * 31 + (size_t)(b)) * 31 + (size_t)(c)) * 31 + (size_t)(d))

/* Maximum array size (prime number for better hash distribution) */
#define TEMP_MAX_SIZE 97

/* Array entry structure */
struct temp_entry {
    int key_a, key_b, key_c, key_d;
    void *value;
    struct temp_entry *next;
};

/* Array structure */
struct temp_array {
    struct temp_entry *buckets[TEMP_MAX_SIZE];
};

/* Initialize temporary array */
#define TEMP_INIT() \
    { { NULL } }

/* Get bucket index from keys */
#define TEMP_BUCKET(arr, a, b, c, d) \
    (TEMP_HASH(a, b, c, d) % TEMP_MAX_SIZE)

/* Set value in temporary array with error handling */
#define TEMP_SET(arr, a, b, c, d, val) do { \
    int _bucket = TEMP_BUCKET(arr, a, b, c, d); \
    struct temp_entry *_entry = malloc(sizeof(struct temp_entry)); \
    if (!_entry) { \
        errno = ENOMEM; \
        break; \
    } \
    _entry->key_a = a; \
    _entry->key_b = b; \
    _entry->key_c = c; \
    _entry->key_d = d; \
    _entry->value = val; \
    _entry->next = (arr).buckets[_bucket]; \
    (arr).buckets[_bucket] = _entry; \
} while (0)

/* Get value from temporary array */
#define TEMP_GET(arr, a, b, c, d) ({ \
    void *_result = NULL; \
    int _bucket = TEMP_BUCKET(arr, a, b, c, d); \
    struct temp_entry *_entry = (arr).buckets[_bucket]; \
    while (_entry) { \
        if (_entry->key_a == (a) && _entry->key_b == (b) && \
            _entry->key_c == (c) && _entry->key_d == (d)) { \
            _result = _entry->value; \
            break; \
        } \
        _entry = _entry->next; \
    } \
    _result; \
})

/* Free temporary array with error handling */
#define TEMP_FREE(arr) do { \
    int _i; \
    for (_i = 0; _i < TEMP_MAX_SIZE; _i++) { \
        struct temp_entry *_entry = (arr).buckets[_i]; \
        while (_entry) { \
            struct temp_entry *_next = _entry->next; \
            if (_entry->value) { \
                free(_entry->value); \
            } \
            free(_entry); \
            _entry = _next; \
        } \
        (arr).buckets[_i] = NULL; \
    } \
} while (0)

/* Declare a function with temporary associative array */
#define TempError int
#define TEMP_SUCCESS 0
#define TEMP_MEMORY_ERROR 1

#define TEMP_FUNC(type, name, param_a, param_b, param_c, param_d) \
type name(int param_a, int param_b, int param_c, int param_d) { \
    struct temp_array _array = TEMP_INIT(); \
    type _result = (type)0; \
    TempError _err = _temp_##name(param_a, param_b, param_c, param_d, &_array); \
    TEMP_FREE(_array); \
    if (_err != TEMP_SUCCESS) { \
        errno = ENOMEM; \
        return _result; \
    } \
    return _result; \
} \
\
TempError _temp_##name(int param_a, int param_b, int param_c, int param_d, struct temp_array *_array)

/* Access array element with parameters */
#define ARR(name, a, b, c, d) ({ \
    type *_ptr = TEMP_GET(*_array, a, b, c, d); \
    if (!_ptr) { \
        _ptr = malloc(sizeof(type)); \
        if (!_ptr) { \
            errno = ENOMEM; \
            break; \
        } \
        *_ptr = (type)0; \
        TEMP_SET(*_array, a, b, c, d, _ptr); \
        if (errno == ENOMEM) { \
            free(_ptr); \
            break; \
        } \
    } \
    _ptr ? *_ptr : (type)0; \
})

/* Set array element with parameters */
#define ARR_SET(name, a, b, c, d, val) do { \
    type *_ptr = TEMP_GET(*_array, a, b, c, d); \
    if (!_ptr) { \
        _ptr = malloc(sizeof(type)); \
        if (!_ptr) { \
            errno = ENOMEM; \
            break; \
        } \
        TEMP_SET(*_array, a, b, c, d, _ptr); \
        if (errno == ENOMEM) { \
            free(_ptr); \
            break; \
        } \
    } \
    *_ptr = val; \
} while (0)

/* Declare a function with temporary associative array and delta transform */
#define TEMP_FUNC_DELTA(type, name, param_a, param_b, param_c, param_d) \
type name(int param_a, int param_b, int param_c, int param_d) { \
    struct temp_array _array = TEMP_INIT(); \
    type _result = (type)0; \
    TempError _err = _temp_##name(param_a, param_b, param_c, param_d, &_array); \
    TEMP_FREE(_array); \
    if (_err != TEMP_SUCCESS) { \
        errno = ENOMEM; \
        return _result; \
    } \
    return _result; \
} \
\
TempError _temp_##name(int param_a, int param_b, int param_c, int param_d, struct temp_array *_array)

/* Apply delta transform to array element */
#define DELTA_TRANSFORM(name, a, b, c, d) do { \
    type _val = ARR(name, a, b, c, d); \
    if (errno == ENOMEM) break; \
    type _prev_a = (a > 0) ? ARR(name, a-1, b, c, d) : 0; \
    if (errno == ENOMEM) break; \
    type _prev_b = (b > 0) ? ARR(name, a, b-1, c, d) : 0; \
    if (errno == ENOMEM) break; \
    type _prev_c = (c > 0) ? ARR(name, a, b, c-1, d) : 0; \
    if (errno == ENOMEM) break; \
    type _prev_d = (d > 0) ? ARR(name, a, b, c, d-1) : 0; \
    if (errno == ENOMEM) break; \
    ARR_SET(name, a, b, c, d, _val + (_prev_a + _prev_b + _prev_c + _prev_d) * 0.1618); \
} while (0)

#endif /* TEMP_ARRAY_H */