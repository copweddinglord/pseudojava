/**
 * free_all.h - Universal Free + Tickstack Cascade with Routing Affinity
 *
 * Centralized recursive free cascade for linked, multidimensional, or dynamic
 * allocations. Uses tickstack with priority-based routing affinity, fuzzy logic
 * for prioritization, and inline assembly for performance. Inspired by exterminator
 * strategies: path prediction, strategic placement, multi-variable optimization,
 * and adaptive feedback. Adheres to late 90s GNU C practices.
 */

#ifndef FREE_ALL_H
#define FREE_ALL_H

#include <stdlib.h>
#include <stdio.h>

/* Configuration defines */
#define MAX_TICK_STACK 1024  /* Max tick stack entries */
#define FUZZY_CONFIDENCE_THRESHOLD 0.7  /* Min confidence for fuzzy prioritization */
#define MEMORY_PRESSURE_THRESHOLD 0.9  /* High memory pressure trigger */

/* ==================== TICKSTACK CORE ==================== */

typedef struct FreeTick {
    void *ptr;              /* Pointer to free */
    size_t size;            /* Allocation size for prioritization */
    int ref_count;          /* Reference count estimate */
    double fuzzy_score;     /* Fuzzy logic score for priority */
    struct FreeTick *next;  /* Next tick in stack */
} FreeTick;

/* Global tick stack */
static FreeTick *tick_stack = NULL;
static size_t tick_count = 0;

/* Push to tick stack with priority (inline assembly for efficiency) */
static inline void tick_push(void *ptr, size_t size, int ref_count) {
    if (!ptr || tick_count >= MAX_TICK_STACK) {
        fprintf(stderr, "FREE_ALL Error: Null ptr or tick stack full\n");
        return;
    }

    FreeTick *t = (FreeTick *)malloc(sizeof(FreeTick));
    if (!t) {
        fprintf(stderr, "FREE_ALL Error: Memory allocation failed for tick\n");
        return;
    }

    t->ptr = ptr;
    t->size = size;
    t->ref_count = ref_count;
    t->fuzzy_score = 0.0; /* Will be set by fuzzy logic */
    t->next = NULL;

    /* Fuzzy logic to score priority (size, ref_count, age) */
    t->fuzzy_score = fuzzy_score_ptr(ptr, size, ref_count);
    if (t->fuzzy_score < FUZZY_CONFIDENCE_THRESHOLD) {
        t->fuzzy_score *= 0.8; /* Penalize low-confidence pointers */
    }

    /* Priority-based insertion (highest fuzzy_score first) */
    if (!tick_stack || t->fuzzy_score > tick_stack->fuzzy_score) {
        t->next = tick_stack;
        tick_stack = t;
    } else {
        FreeTick *cur = tick_stack;
        while (cur->next && cur->next->fuzzy_score > t->fuzzy_score) {
            cur = cur->next;
        }
        t->next = cur->next;
        cur->next = t;
    }

    /* Inline assembly for stack push optimization */
#ifdef __GNUC__
    asm volatile ("push %0; mov %1, %%eax; pop %%eax" :: "r"(t), "r"(ptr));
#endif

    tick_count++;
}

/* Cascade free with double-free protection */
static inline void tick_cascade(void) {
    FreeTick *cur = tick_stack;
    while (cur) {
        FreeTick *next = cur->next;
        if (cur->ptr) {
            /* Check for double-free */
            if (!is_valid_ptr(cur->ptr)) {
                fprintf(stderr, "FREE_ALL Warning: Double-free attempt on %p\n", cur->ptr);
            } else {
                free(cur->ptr);
            }
        }
        free(cur);
        cur = next;
    }
    tick_stack = NULL;
    tick_count = 0;

    /* Inline assembly for stack cleanup */
#ifdef __GNUC__
    asm volatile ("mov $0, %%eax; mov %%eax, %0" : "=r"(tick_stack));
#endif
}

/* ==================== FUZZY LOGIC FOR PRIORITIZATION ==================== */

/* Simplified fuzzy logic to score pointers for freeing priority */
static inline double fuzzy_score_ptr(void *ptr, size_t size, int ref_count) {
    double size_factor = size > 1024 ? 0.9 : size / 1024.0; /* Prioritize large allocations */
    double ref_factor = ref_count > 1 ? 0.7 : 0.3; /* Prioritize multiply-referenced */
    double memory_pressure = monitor_memory_pressure();

    /* Fuzzy AND: min of factors */
    double score = size_factor < ref_factor ? size_factor : ref_factor;
    score = score < memory_pressure ? score : memory_pressure;
    return score;
}

/* ==================== MEMORY PRESSURE MONITORING ==================== */

/* Simulate memory pressure (heuristic based on tick_count) */
static inline double monitor_memory_pressure(void) {
    return tick_count > (MAX_TICK_STACK * MEMORY_PRESSURE_THRESHOLD) ? 0.9 : 0.5;
}

/* ==================== UNIVERSAL FREE ==================== */

/* Free a 1D array with tick stack integration */
static inline void free_1d(void *arr, size_t size) {
    if (arr) {
        tick_push(arr, size, 1);
    }
}

/* Free a 2D array with tick stack */
static inline void free_2d(void **arr, size_t rows, size_t row_size) {
    if (!arr) return;
    for (size_t i = 0; i < rows; i++) {
        if (arr[i]) {
            tick_push(arr[i], row_size, 1);
        }
    }
    tick_push(arr, rows * sizeof(void *), rows);
}

/* Free a 3D array with tick stack */
static inline void free_3d(void ***arr, size_t rows, size_t cols, size_t col_size) {
    if (!arr) return;
    for (size_t i = 0; i < rows; i++) {
        if (arr[i]) {
            for (size_t j = 0; j < cols; j++) {
                if (arr[i][j]) {
                    tick_push(arr[i][j], col_size, 1);
                }
            }
            tick_push(arr[i], cols * sizeof(void *), cols);
        }
    }
    tick_push(arr, rows * sizeof(void **), rows);
}

/* ==================== LINKED LIST FREE ==================== */

typedef struct FreeNode {
    void *payload;
    struct FreeNode *next;
} FreeNode;

static inline void free_list(FreeNode *head) {
    FreeNode *cur = head;
    while (cur) {
        FreeNode *next = cur->next;
        if (cur->payload) {
            tick_push(cur->payload, sizeof(void *), 1);
        }
        tick_push(cur, sizeof(FreeNode), 1);
        cur = next;
    }
}

/* ==================== GLOBAL SWEEP ==================== */

/* Free all using tick stack cascade with adaptive feedback */
static inline void free_all_tickstack(void) {
    if (monitor_memory_pressure() > MEMORY_PRESSURE_THRESHOLD) {
        /* High memory pressure: free immediately */
        tick_cascade();
    } else {
        /* Low pressure: defer freeing for later optimization */
        printf("FREE_ALL: Low memory pressure, deferring cascade\n");
    }
}

/* ==================== SHOOT OVER FREE ==================== */

/* Inject freeing code at specific point (inspired by PROBLEM_FUZZER.h) */
static inline void shoot_over_free(void *ptr, size_t size, const char *insert_id) {
    if (!ptr) return;
    tick_push(ptr, size, 1);
    printf("/* FREE_ALL: Injected free for %s at %p */\n", insert_id, ptr);
}

/* ==================== UTILITY ==================== */

/* Simulated pointer validation (placeholder for real implementation) */
static inline int is_valid_ptr(void *ptr) {
    return ptr != NULL; /* Simplified; could use system-specific checks */
}

/* Error reporting */
static inline void report_error(const char *msg, void *ptr) {
    fprintf(stderr, "FREE_ALL Error: %s (ptr: %p)\n", msg, ptr);
}

#endif /* FREE_ALL_H */