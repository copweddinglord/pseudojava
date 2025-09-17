/**
 * fixed_problem_fuzzer.h - Enhanced Problem Processing System
 *
 * Processes mathematical problems with fuzzy logic dialectic
 * and robust error handling for C89/C99 compatibility.
 */

#ifndef FIXED_PROBLEM_FUZZER_H
#define FIXED_PROBLEM_FUZZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Configuration */
#define MAX_JSON_SIZE          1024
#define MAX_PROBLEMS           100
#define MAX_INSERT_ID          64
#define MAX_SOLUTION_SIZE      256
#define MAX_SYMBOLS            128
#define FUZZY_ITER_MAX         1000

/* Error codes */
#define PF_SUCCESS             0
#define PF_ERR_NULL_POINTER    401
#define PF_ERR_MEMORY_ALLOC    402
#define PF_ERR_INVALID_JSON    403
#define PF_ERR_NO_INSERT       404
#define PF_ERR_NUMERICAL       405

/* Structure for JSON problem */
typedef struct {
    char json[MAX_JSON_SIZE];
    char insert_id[MAX_INSERT_ID];
    int resolved;
    char solution[MAX_SOLUTION_SIZE];
} Problem;

/* Structure for fuzzy agent */
typedef struct {
    int tone;
    double confidence;
    char proposal[MAX_SOLUTION_SIZE];
} Agent;

/* Structure for problem processing */
typedef struct {
    Problem problems[MAX_PROBLEMS];
    int problem_count;
    int error_code;
} ProblemFuzzer;

/* Function declarations */
int init_problem_fuzzer(ProblemFuzzer* fuzzer);
int add_problem(ProblemFuzzer* fuzzer, const char* json, const char* insert_id);
int process_problems(ProblemFuzzer* fuzzer);
void cleanup_problem_fuzzer(ProblemFuzzer* fuzzer);

#endif /* FIXED_PROBLEM_FUZZER_H */