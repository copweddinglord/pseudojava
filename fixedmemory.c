#include "fixed_memory_systems.h"

// Create a memory system
UnifiedMemorySystem memory;
init_memory_system(&memory);

// Create a memory region
MemoryRegion* region = create_memory_region(10, MEM_REGION_LIVING);
add_memory_value(region, 42.0, "answer");

// Get value (will have slight variation due to living number generation)
double val = get_memory_value(region, 0);
