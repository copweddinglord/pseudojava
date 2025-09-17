#include <stdio.h>
#include "fixed_memory_systems.h"

int main() {
    printf("=== Memory System Example ===\n\n");
    
    // Initialize memory system
    UnifiedMemorySystem memory;
    init_memory_system(&memory);
    
    // Create memory regions of different types
    MemoryRegion* normal_region = create_memory_region(5, MEM_REGION_NORMAL);
    MemoryRegion* living_region = create_memory_region(5, MEM_REGION_LIVING);
    
    // Add values to regions
    printf("Adding values to memory regions...\n");
    add_memory_value(normal_region, 42.0, "Answer to everything");
    add_memory_value(normal_region, 3.14159, "Pi");
    add_memory_value(living_region, 1.618, "Golden ratio");
    add_memory_value(living_region, 2.71828, "Euler's number");
    
    // Retrieve and display values
    printf("\nRetrieving values from normal region:\n");
    printf("Value 0: %.5f\n", get_memory_value(normal_region, 0));
    printf("Value 1: %.5f\n", get_memory_value(normal_region, 1));
    
    printf("\nRetrieving values from living region (notice slight variations):\n");
    printf("Value 0 (first access): %.5f\n", get_memory_value(living_region, 0));
    printf("Value 0 (second access): %.5f\n", get_memory_value(living_region, 0));
    printf("Value 0 (third access): %.5f\n", get_memory_value(living_region, 0));
    printf("Value 1: %.5f\n", get_memory_value(living_region, 1));
    
    // Clean up
    cleanup_memory_system(&memory);
    printf("\nMemory system cleaned up successfully.\n");
    
    return 0;
}
