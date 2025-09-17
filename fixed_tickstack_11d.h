/**
 * tickstack_11d.h - 11D TickStack with Enhanced Error Handling
 *
 * Implements 11-dimensional tick stack structures and operations
 * with robust error handling and memory management.
 */

#ifndef TICKSTACK_11D_H
#define TICKSTACK_11D_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ==================== ERROR CODES ==================== */
#define TS_SUCCESS                  0
#define TS_ERR_NULL_POINTER         101
#define TS_ERR_INVALID_DIMENSION    102
#define TS_ERR_STACK_OVERFLOW       103
#define TS_ERR_STACK_UNDERFLOW      104
#define TS_ERR_INVALID_OPERATION    105
#define TS_ERR_NUMERICAL_INSTABILITY 106
#define TS_ERR_MEMORY_ALLOCATION    107
#define TS_ERR_INVALID_INDEX        108

/* ==================== CONSTANTS ==================== */
#define TICKSTACK_DIM      11
#define TICKSTACK_SIZE     50
#define TICKSTACK_PHI      1.618033988749895
#define TICKSTACK_THIRD    0.333333333333
#define TICKSTACK_EPSILON  1e-10

/* ==================== DATA STRUCTURES ==================== */
typedef struct {
    double coords[TICKSTACK_DIM];
    int is_observed;
} ElectronFreeLiteral;

typedef struct {
    double deltas[TICKSTACK_DIM];
    double magnitude;
    double directions[TICKSTACK_DIM];
    double phase;
    double harmonic_factor;
    int variable_count;
} MultivarDeltaTopology;

typedef struct {
    double delta;
    double magnitude;
    double direction;
    double phase;
    double harmonic_factor;
} LinearDelta;

/* Tick stack data structure for multivariable systems */
typedef struct {
    ElectronFreeLiteral* literals;
    size_t literal_count;
    double probabilities[TICKSTACK_SIZE];
    MultivarDeltaTopology* delta_topology;
    size_t topology_count;
    int error_code;
} MultivarTickStack;

/* Tick stack data structure for linear systems */
typedef struct {
    double* values;
    size_t value_count;
    double probabilities[TICKSTACK_SIZE];
    LinearDelta* linear_deltas;
    size_t delta_count;
    int error_code;
} LinearTickStack;

/* Function declarations */
void init_multivar_tickstack(MultivarTickStack* ts);
void init_linear_tickstack(LinearTickStack* ts);
void push_electron_free_literal(MultivarTickStack* ts, const double* coords);
void push_linear_value(LinearTickStack* ts, double value);
double vector_magnitude(const double* vector);
void compute_multivar_delta_topology(MultivarTickStack* ts);
void compute_linear_deltas(LinearTickStack* ts);
void x_operator_multivar(MultivarTickStack* ts, int operation);
void x_operator_linear(LinearTickStack* ts, int operation);
void string_to_11d_coord(const char* str, double* coord);

#endif /* TICKSTACK_11D_H */