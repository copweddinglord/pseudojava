#include "fixed_tickstack_11d.h"

// Create a tick stack
MultivarTickStack stack;
init_multivar_tickstack(&stack);

// Add a point in 11D space
double coords[TICKSTACK_DIM] = {1,2,3,4,5,6,7,8,9,10,11};
push_electron_free_literal(&stack, coords);

// Calculate delta topology
compute_multivar_delta_topology(&stack);
