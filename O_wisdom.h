/**
 * O_wisdom.h - Big O complexity analyzer and evaluator
 *
 * Analyzes algorithm runtime and space complexity, evaluates acceptability
 * for various contexts, and provides visualizations.
 */

#ifndef O_WISDOM_H
#define O_WISDOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/* Complexity types */
typedef enum {
    O_CONSTANT,       /* O(1) */
    O_LOGARITHMIC,    /* O(log n) */
    O_LINEAR,         /* O(n) */
    O_LINEARITHMIC,   /* O(n log n) */
    O_QUADRATIC,      /* O(n²) */
    O_CUBIC,          /* O(n³) */
    O_POLYNOMIAL,     /* O(n^k) where k > 3 */
    O_EXPONENTIAL,    /* O(2^n) */
    O_FACTORIAL       /* O(n!) */
} ComplexityType;

/* Evaluation contexts */
typedef enum {
    CTX_REALTIME,     /* Real-time systems, must be very fast */
    CTX_INTERACTIVE,  /* Interactive applications, user-facing */
    CTX_BATCH,        /* Batch processing, can be slower */
    CTX_PREPROCESSING,/* One-time preprocessing, can be slow */
    CTX_THEORETICAL   /* Theoretical analysis only */
} EvaluationContext;

/* Operation details */
typedef struct {
    ComplexityType time_complexity;
    ComplexityType space_complexity;
    double coefficient;           /* Leading coefficient */
    int polynomial_degree;        /* For O(n^k), the value of k */
    char *description;            /* Human-readable description */
    unsigned long long steps_at_n[5]; /* Steps at n=10,100,1000,10000,100000 */
} Operation;

/* O_wisdom structure */
typedef struct {
    Operation op;
    EvaluationContext context;
    bool (*is_acceptable)(struct O_wisdom *); /* Renamed for clarity */
    void (*explain)(struct O_wisdom *);
    void (*visualize)(struct O_wisdom *);
} O_wisdom;

/* Function prototypes */
O_wisdom *O_wisdom_create(ComplexityType time_complexity, double coefficient);
void O_wisdom_destroy(O_wisdom *wisdom);
bool O_wisdom_is_acceptable(O_wisdom *wisdom);
void O_wisdom_explain(O_wisdom *wisdom);
void O_wisdom_visualize(O_wisdom *wisdom);
void O_wisdom_set_context(O_wisdom *wisdom, EvaluationContext context);
void O_wisdom_set_space_complexity(O_wisdom *wisdom, ComplexityType space_complexity);
void O_wisdom_set_polynomial_degree(O_wisdom *wisdom, int degree);
void O_wisdom_set_description(O_wisdom *wisdom, const char *description);

/* Implementation */

/* Calculate steps for a given n and complexity */
static unsigned long long calculate_steps(ComplexityType complexity, double coefficient, int degree, unsigned long long n) {
    if (coefficient <= 0 || n == 0) return 0;

    switch (complexity) {
        case O_CONSTANT:
            return (unsigned long long)coefficient;
        case O_LOGARITHMIC:
            return (unsigned long long)(coefficient * log2((double)n));
        case O_LINEAR:
            return (unsigned long long)(coefficient * n);
        case O_LINEARITHMIC:
            return (unsigned long long)(coefficient * n * log2((double)n));
        case O_QUADRATIC:
            return (unsigned long long)(coefficient * n * n);
        case O_CUBIC:
            return (unsigned long long)(coefficient * n * n * n);
        case O_POLYNOMIAL:
            return degree < 1 ? 0 : (unsigned long long)(coefficient * pow((double)n, degree));
        case O_EXPONENTIAL:
            return n > 63 ? ULLONG_MAX : (unsigned long long)(coefficient * (1ULL << n));
        case O_FACTORIAL: {
            if (n > 20) return ULLONG_MAX;
            unsigned long long fact = 1;
            for (unsigned long long i = 2; i <= n; i++) {
                if (fact > ULLONG_MAX / i) return ULLONG_MAX;
                fact *= i;
            }
            return (unsigned long long)(coefficient * fact);
        }
        default:
            return 0;
    }
}

/* Get complexity name */
static const char *get_complexity_name(ComplexityType complexity) {
    switch (complexity) {
        case O_CONSTANT:    return "O(1)";
        case O_LOGARITHMIC: return "O(log n)";
        case O_LINEAR:      return "O(n)";
        case O_LINEARITHMIC:return "O(n log n)";
        case O_QUADRATIC:   return "O(n²)";
        case O_CUBIC:       return "O(n³)";
        case O_POLYNOMIAL:  return "O(n^k)";
        case O_EXPONENTIAL: return "O(2^n)";
        case O_FACTORIAL:   return "O(n!)";
        default:            return "Unknown";
    }
}

/* Create a new O_wisdom instance */
O_wisdom *O_wisdom_create(ComplexityType time_complexity, double coefficient) {
    if (coefficient <= 0) return NULL;

    O_wisdom *wisdom = malloc(sizeof(O_wisdom));
    if (!wisdom) return NULL;

    wisdom->op.time_complexity = time_complexity;
    wisdom->op.space_complexity = O_LINEAR;
    wisdom->op.coefficient = coefficient;
    wisdom->op.polynomial_degree = 4;
    wisdom->op.description = strdup("Algorithm complexity analysis");
    if (!wisdom->op.description) {
        free(wisdom);
        return NULL;
    }
    wisdom->context = CTX_INTERACTIVE;

    /* Calculate steps for different n values */
    unsigned long long n_values[] = {10, 100, 1000, 10000, 100000};
    for (int i = 0; i < 5; i++) {
        wisdom->op.steps_at_n[i] = calculate_steps(time_complexity, coefficient, wisdom->op.polynomial_degree, n_values[i]);
    }

    /* Assign function pointers */
    wisdom->is_acceptable = O_wisdom_is_acceptable;
    wisdom->explain = O_wisdom_explain;
    wisdom->visualize = O_wisdom_visualize;

    return wisdom;
}

/* Free O_wisdom resources */
void O_wisdom_destroy(O_wisdom *wisdom) {
    if (!wisdom) return;
    free(wisdom->op.description);
    free(wisdom);
}

/* Determine if complexity is acceptable for the given context */
bool O_wisdom_is_acceptable(O_wisdom *wisdom) {
    if (!wisdom) return false;

    switch (wisdom->context) {
        case CTX_REALTIME:
            return wisdom->op.time_complexity <= O_LOGARITHMIC ||
                   (wisdom->op.time_complexity == O_LINEAR && wisdom->op.coefficient < 0.01);
        case CTX_INTERACTIVE:
            return wisdom->op.time_complexity <= O_LINEAR ||
                   (wisdom->op.time_complexity == O_LINEARITHMIC && wisdom->op.coefficient < 0.1);
        case CTX_BATCH:
            return wisdom->op.time_complexity <= O_LINEARITHMIC ||
                   (wisdom->op.time_complexity == O_QUADRATIC && wisdom->op.coefficient < 0.01);
        case CTX_PREPROCESSING:
            return wisdom->op.time_complexity <= O_QUADRATIC ||
                   (wisdom->op.time_complexity == O_CUBIC && wisdom->op.coefficient < 0.001);
        case CTX_THEORETICAL:
            return true;
        default:
            return false;
    }
}

/* Explain the complexity and its implications */
void O_wisdom_explain(O_wisdom *wisdom) {
    if (!wisdom) {
        printf("Error: Invalid wisdom object\n");
        return;
    }

    printf("=== Complexity Analysis ===\n");
    printf("Algorithm: %s\n", wisdom->op.description ? wisdom->op.description : "N/A");
    printf("Time Complexity: %s", get_complexity_name(wisdom->op.time_complexity));
    if (wisdom->op.time_complexity == O_POLYNOMIAL) {
        printf(" (k=%d)", wisdom->op.polynomial_degree);
    }
    if (wisdom->op.coefficient != 1.0) {
        printf(" (coefficient: %.3f)", wisdom->op.coefficient);
    }
    printf("\nSpace Complexity: %s\n", get_complexity_name(wisdom->op.space_complexity));

    printf("Context: ");
    switch (wisdom->context) {
        case CTX_REALTIME:     printf("Real-time systems\n"); break;
        case CTX_INTERACTIVE:  printf("Interactive applications\n"); break;
        case CTX_BATCH:        printf("Batch processing\n"); break;
        case CTX_PREPROCESSING:printf("One-time preprocessing\n"); break;
        case CTX_THEORETICAL:  printf("Theoretical analysis\n"); break;
        default:               printf("Unknown\n"); break;
    }

    printf("\nApproximate steps:\n");
    const char *n_labels[] = {"n=10", "n=100", "n=1,000", "n=10,000", "n=100,000"};
    for (int i = 0; i < 5; i++) {
        printf("  %s: %llu steps\n", n_labels[i], wisdom->op.steps_at_n[i]);
    }

    printf("\nAcceptability: %s\n", wisdom->is_acceptable(wisdom) ? "ACCEPTABLE ✓" : "NOT ACCEPTABLE ✗");
    if (!wisdom->is_acceptable(wisdom)) {
        printf("\nSuggested improvements:\n");
        switch (wisdom->op.time_complexity) {
            case O_FACTORIAL:
            case O_EXPONENTIAL:
                printf("- Use dynamic programming\n- Explore greedy or approximation algorithms\n- Reformulate the problem\n");
                break;
            case O_POLYNOMIAL:
            case O_CUBIC:
                printf("- Optimize data structures\n- Use divide-and-conquer\n- Consider preprocessing\n");
                break;
            case O_QUADRATIC:
                printf("- Use hash tables\n- Implement sorting or binary search\n- Solve incrementally\n");
                break;
            case O_LINEARITHMIC:
                printf("- Evaluate if sorting is necessary\n- Seek linear-time alternatives\n");
                break;
            default:
                printf("- Optimize constants\n- Explore algorithm-specific improvements\n");
                break;
        }
    }
}

/* Visualize the growth rate */
void O_wisdom_visualize(O_wisdom *wisdom) {
    if (!wisdom) {
        printf("Error: Invalid wisdom object\n");
        return;
    }

    printf("=== Growth Rate Visualization ===\n");
    const int width = 50;
    unsigned long long max_steps = 0;

    /* Find maximum for scaling */
    for (int i = 0; i < 5; i++) {
        if (wisdom->op.steps_at_n[i] > max_steps && wisdom->op.steps_at_n[i] != ULLONG_MAX) {
            max_steps = wisdom->op.steps_at_n[i];
        }
    }
    max_steps = max_steps > 1000000000 ? 1000000000 : (max_steps == 0 ? 1 : max_steps);

    /* Print bars */
    const char *n_values[] = {"n=10", "n=100", "n=1K", "n=10K", "n=100K"};
    for (int i = 0; i < 5; i++) {
        unsigned long long steps = wisdom->op.steps_at_n[i] > 1000000000 ? 1000000000 : wisdom->op.steps_at_n[i];
        int bar_width = max_steps ? (steps * width) / max_steps : 0;
        printf("%-6s |", n_values[i]);
        for (int j = 0; j < bar_width && j < width; j++) printf("#");
        printf(" ");
        if (steps >= 1000000000) printf(">1B steps\n");
        else if (steps >= 1000000) printf("%.1fM steps\n", steps / 1000000.0);
        else if (steps >= 1000) printf("%.1fK steps\n", steps / 1000.0);
        else printf("%llu steps\n", steps);
    }

    printf("\nComplexity Class Comparison:\n");
    const char *classes[] = {"O(1)", "O(log n)", "O(n)", "O(n log n)", "O(n²)", "O(n³)", "O(n^k)", "O(2^n)", "O(n!)"};
    for (int i = 0; i < 9; i++) {
        printf("%-10s: %s\n", classes[i], i == wisdom->op.time_complexity ? "--> CURRENT <--" :
                               i < wisdom->op.time_complexity ? "BETTER" : "WORSE");
    }
}

/* Set evaluation context */
void O_wisdom_set_context(O_wisdom *wisdom, EvaluationContext context) {
    if (wisdom) wisdom->context = context;
}

/* Set space complexity */
void O_wisdom_set_space_complexity(O_wisdom *wisdom, ComplexityType space_complexity) {
    if (wisdom) wisdom->op.space_complexity = space_complexity;
}

/* Set polynomial degree for O(n^k) */
void O_wisdom_set_polynomial_degree(O_wisdom *wisdom, int degree) {
    if (!wisdom || degree < 1) return;
    wisdom->op.polynomial_degree = degree;
    if (wisdom->op.time_complexity == O_POLYNOMIAL) {
        unsigned long long n_values[] = {10, 100, 1000, 10000, 100000};
        for (int i = 0; i < 5; i++) {
            wisdom->op.steps_at_n[i] = calculate_steps(O_POLYNOMIAL, wisdom->op.coefficient, degree, n_values[i]);
        }
    }
}

/* Set algorithm description */
void O_wisdom_set_description(O_wisdom *wisdom, const char *description) {
    if (!wisdom || !description) return;
    char *new_desc = strdup(description);
    if (!new_desc) return;
    free(wisdom->op.description);
    wisdom->op.description = new_desc;
}

#endif /* O_WISDOM_H */