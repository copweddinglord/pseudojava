#include "higgs_king.h"

// Create a Higgs field
HiggsField field;
init_higgs_field(&field);

// Add field points
double coords[HIGGS_FIELD_DIM] = {1,2,3,4,5,6,7,8,9,10,11};
add_higgs_point(&field, coords, 1.0, "point1");

// Simulate interaction
higgs_interaction(&field);
