/**
 * Junk Warrior - Conservative 2D Fuzzy Logic System
 *
 * A lightweight 2D fuzzy logic system designed as a conservative junk collector.
 * Processes data slowly and safely to avoid crashes, with a focus on stability.
 */

#ifndef JUNK_WARRIOR_H
#define JUNK_WARRIOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* Constants */
#define JUNK_WARRIOR_DIM 2              /* 2D system dimension */
#define JUNK_WARRIOR_SLOW_RATE 0.1      /* Slow processing rate */
#define JUNK_WARRIOR_CONSERVATIVE_FACTOR 0.9  /* Conservative processing factor */
#define JUNK_WARRIOR_MAX_JUNK 100       /* Maximum junk items */
#define JUNK_WARRIOR_TOLERANCE 1e-6     /* Tolerance for fuzzy comparisons */

/* Data structures */

/* Fuzzy value in 2D space */
typedef struct {
    double value[JUNK_WARRIOR_DIM];
    double certainty;
    char* label;
} FuzzyValue;

/* Junk item */
typedef struct {
    FuzzyValue fuzz;
    int priority;
    time_t timestamp;
    int processed;
} JunkItem;

/* Junk warrior system */
typedef struct {
    JunkItem junk_items[JUNK_WARRIOR_MAX_JUNK];
    size_t junk_count;
    double processing_rate;
    int conservative_mode;
    double health;
} JunkWarrior;

/* Global instance */
static JunkWarrior junk_warrior;

/* Function declarations */
void init_junk_warrior(void);
FuzzyValue create_fuzzy_value(double x, double y, double certainty, const char* label);
void add_junk_item(FuzzyValue fuzz, int priority);
void process_junk_conservatively(void);
void get_junk_warrior_status(void);
double fuzzy_and(double a, double b);
double fuzzy_or(double a, double b);
double fuzzy_not(double a);
double fuzzy_implies(double a, double b);
double fuzzy_equivalence(double a, double b);
double conservative_processing(double value);
int safe_junk_processing(JunkItem* item);
void maintain_system_health(void);

/* Macros */
#define JUNK_WARRIOR_INIT() init_junk_warrior()
#define FUZZY_VALUE(x, y, cert, name) create_fuzzy_value((x), (y), (cert), (name))
#define JUNK_WARRIOR_ADD(fuzz, priority) add_junk_item((fuzz), (priority))
#define JUNK_WARRIOR_PROCESS() process_junk_conservatively()
#define JUNK_WARRIOR_STATUS() get_junk_warrior_status()

/* Function implementations */

/* Initialize the junk warrior system */
void init_junk_warrior(void) {
    junk_warrior.junk_count = 0;
    junk_warrior.processing_rate = JUNK_WARRIOR_SLOW_RATE;
    junk_warrior.conservative_mode = 1;
    junk_warrior.health = 1.0;

    for (size_t i = 0; i < JUNK_WARRIOR_MAX_JUNK; i++) {
        junk_warrior.junk_items[i].fuzz.value[0] = 0.0;
        junk_warrior.junk_items[i].fuzz.value[1] = 0.0;
        junk_warrior.junk_items[i].fuzz.certainty = 0.0;
        junk_warrior.junk_items[i].fuzz.label = NULL;
        junk_warrior.junk_items[i].priority = 0;
        junk_warrior.junk_items[i].timestamp = 0;
        junk_warrior.junk_items[i].processed = 0;
    }

    srand((unsigned int)time(NULL));
}

/* Create a fuzzy value */
FuzzyValue create_fuzzy_value(double x, double y, double certainty, const char* label) {
    FuzzyValue fuzz = {
        .value = {x, y},
        .certainty = (certainty < 0.0) ? 0.0 : (certainty > 1.0) ? 1.0 : certainty,
        .label = (label != NULL) ? strdup(label) : NULL
    };
    if (label != NULL && fuzz.label == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in create_fuzzy_value\n");
    }
    return fuzz;
}

/* Add a junk item to the system */
void add_junk_item(FuzzyValue fuzz, int priority) {
    if (junk_warrior.junk_count >= JUNK_WARRIOR_MAX_JUNK) {
        fprintf(stderr, "Warning: Maximum junk capacity reached\n");
        return;
    }

    size_t index = junk_warrior.junk_count++;
    junk_warrior.junk_items[index].fuzz = fuzz;
    junk_warrior.junk_items[index].priority = priority;
    junk_warrior.junk_items[index].timestamp = time(NULL);
    junk_warrior.junk_items[index].processed = 0;
}

/* Process junk items conservatively */
void process_junk_conservatively(void) {
    maintain_system_health();

    if (junk_warrior.health < 0.2) {
        fprintf(stderr, "Warning: System health critical - pausing processing\n");
        return;
    }

    size_t processed_count = 0;
    for (size_t i = 0; i < junk_warrior.junk_count; i++) {
        if (junk_warrior.junk_items[i].processed) continue;

        if (safe_junk_processing(&junk_warrior.junk_items[i])) {
            junk_warrior.junk_items[i].processed = 1;
            processed_count++;
        }

        if (processed_count >= (size_t)(junk_warrior.processing_rate * 10)) {
            break;
        }
    }

    printf("Junk Warrior: Processed %zu junk items conservatively\n", processed_count);
}

/* Get the status of the junk warrior */
void get_junk_warrior_status(void) {
    printf("=== Junk Warrior Status ===\n");
    printf("Junk Items: %zu/%d\n", junk_warrior.junk_count, JUNK_WARRIOR_MAX_JUNK);
    printf("Processing Rate: %.2f\n", junk_warrior.processing_rate);
    printf("Conservative Mode: %s\n", junk_warrior.conservative_mode ? "ON" : "OFF");
    printf("System Health: %.2f\n", junk_warrior.health);

    if (junk_warrior.junk_count > 0) {
        printf("\nRecent Junk Items:\n");
        size_t show_count = (junk_warrior.junk_count < 5) ? junk_warrior.junk_count : 5;
        for (size_t i = 0; i < show_count; i++) {
            const char* label = junk_warrior.junk_items[i].fuzz.label ? 
                               junk_warrior.junk_items[i].fuzz.label : "Unnamed";
            printf("  [%zu] %s: (%.2f, %.2f) [Priority: %d, Certainty: %.2f]\n",
                   i, label,
                   junk_warrior.junk_items[i].fuzz.value[0],
                   junk_warrior.junk_items[i].fuzz.value[1],
                   junk_warrior.junk_items[i].priority,
                   junk_warrior.junk_items[i].fuzz.certainty);
        }
    }
}

/* Fuzzy logic operations */
double fuzzy_and(double a, double b) {
    return (a < b) ? a : b; /* Minimum operation */
}

double fuzzy_or(double a, double b) {
    return (a > b) ? a : b; /* Maximum operation */
}

double fuzzy_not(double a) {
    return 1.0 - a;
}

double fuzzy_implies(double a, double b) {
    return fuzzy_or(fuzzy_not(a), b);
}

double fuzzy_equivalence(double a, double b) {
    return fuzzy_and(fuzzy_implies(a, b), fuzzy_implies(b, a));
}

/* Apply conservative processing to a value */
double conservative_processing(double value) {
    return value * JUNK_WARRIOR_CONSERVATIVE_FACTOR;
}

/* Process a junk item safely */
int safe_junk_processing(JunkItem* item) {
    if (item == NULL) {
        fprintf(stderr, "Warning: Null junk item detected, skipping\n");
        return 0;
    }

    if (isnan(item->fuzz.value[0]) || isnan(item->fuzz.value[1]) ||
        isinf(item->fuzz.value[0]) || isinf(item->fuzz.value[1])) {
        fprintf(stderr, "Warning: Invalid values in junk item, skipping\n");
        return 0;
    }

    item->fuzz.value[0] = conservative_processing(item->fuzz.value[0]);
    item->fuzz.value[1] = conservative_processing(item->fuzz.value[1]);
    item->fuzz.certainty *= 0.95;
    return 1;
}

/* Maintain system health */
void maintain_system_health(void) {
    if (junk_warrior.health < 1.0) {
        junk_warrior.health += 0.01;
    }
    junk_warrior.health = (junk_warrior.health < 0.0) ? 0.0 :
                          (junk_warrior.health > 1.0) ? 1.0 : junk_warrior.health;
}

#endif /* JUNK_WARRIOR_H */