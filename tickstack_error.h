/**
 * tickstack_error.h - Error Handling for TickStack Framework
 *
 * Provides error handling with verbosity levels, recovery strategies,
 * logging, and history. Balances performance with robustness.
 */

#ifndef TICKSTACK_ERROR_H
#define TICKSTACK_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

/* ==================== CONFIGURATION ==================== */
#define TICKSTACK_MAX_ERROR_MSG     256
#define TICKSTACK_MAX_ERROR_HISTORY 16
#define TICKSTACK_ERROR_LOG_FILE    "tickstack_error.log"

/* ==================== ENUMERATIONS ==================== */
typedef enum {
    TS_INFO = 0,
    TS_WARNING,
    TS_ERROR,
    TS_FATAL
} TickStackErrorLevel;

typedef enum {
    TS_MEMORY = 0,
    TS_MATH,
    TS_IO,
    TS_LOGIC,
    TS_SYSTEM,
    TS_USER,
    TS_UNKNOWN
} TickStackErrorCategory;

typedef enum {
    TS_STRATEGY_ABORT = 0,
    TS_STRATEGY_RETRY,
    TS_STRATEGY_FALLBACK,
    TS_STRATEGY_IGNORE
} TickStackRecoveryStrategy;

/* ==================== STRUCTURES ==================== */
typedef struct {
    const char *file;
    int line;
    const char *function;
    void *context_data;
} TickStackErrorContext;

typedef struct {
    TickStackErrorLevel level;
    TickStackErrorCategory category;
    int code;
    char message[TICKSTACK_MAX_ERROR_MSG];
    TickStackErrorContext context;
    time_t timestamp;
    TickStackRecoveryStrategy strategy;
} TickStackError;

typedef struct {
    TickStackError current;
    TickStackError history[TICKSTACK_MAX_ERROR_HISTORY];
    size_t history_count;
    size_t history_index;
    FILE *log_file;
    int verbosity;
    int (*error_callback)(TickStackError *);
} TickStackErrorState;

/* ==================== GLOBAL STATE ==================== */
static TickStackErrorState ts_error_state = {
    .history_count = 0,
    .history_index = 0,
    .log_file = NULL,
    .verbosity = 2,
    .error_callback = NULL
};

/* ==================== API FUNCTIONS ==================== */
void ts_error_init(int verbosity);
void ts_error_cleanup(void);
void ts_error_set_callback(int (*callback)(TickStackError *));
void ts_error_report(TickStackErrorLevel level, TickStackErrorCategory category,
                     int code, const char *file, int line, const char *function,
                     const char *format, ...);
const TickStackError *ts_error_get_current(void);
const TickStackError *ts_error_get_history(size_t *count);
void ts_error_clear(void);
int ts_error_check(TickStackErrorLevel min_level);
const char *ts_error_level_str(TickStackErrorLevel level);
const char *ts_error_category_str(TickStackErrorCategory category);
void ts_error_print(FILE *stream, const TickStackError *error);
void ts_error_dump_history(FILE *stream);
void ts_error_set_strategy(TickStackErrorCategory category, int code,
                           TickStackRecoveryStrategy strategy);
TickStackRecoveryStrategy ts_error_get_strategy(void);

/* ==================== CONVENIENCE MACROS ==================== */
#define TS_INFO_MSG(category, code, ...) \
    ts_error_report(TS_INFO, category, code, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define TS_WARNING_MSG(category, code, ...) \
    ts_error_report(TS_WARNING, category, code, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define TS_ERROR_MSG(category, code, ...) \
    ts_error_report(TS_ERROR, category, code, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define TS_FATAL_MSG(category, code, ...) \
    ts_error_report(TS_FATAL, category, code, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define TS_MALLOC(size)        ts_malloc(size, __FILE__, __LINE__, __func__)
#define TS_CALLOC(count, size) ts_calloc(count, size, __FILE__, __LINE__, __func__)
#define TS_REALLOC(ptr, size)  ts_realloc(ptr, size, __FILE__, __LINE__, __func__)
#define TS_FREE(ptr)           ts_free(ptr)

#define TS_ASSERT(cond, category, code, msg) \
    do { \
        if (!(cond)) { \
            TS_ERROR_MSG(category, code, msg); \
            assert(cond); \
        } \
    } while (0)

/* ==================== MEMORY HELPERS ==================== */
void *ts_malloc(size_t size, const char *file, int line, const char *function);
void *ts_calloc(size_t count, size_t size, const char *file, int line, const char *function);
void *ts_realloc(void *ptr, size_t size, const char *file, int line, const char *function);
void ts_free(void *ptr);

#endif /* TICKSTACK_ERROR_H */
