/* energy.c - Energy calculation */
#include <stdio.h>
#include <string.h>
#include "energy.h"

EnergyModel energy_model;

void init_energy_model() {
    // Based on ARM Cortex-M4 @ 168 MHz
    energy_model.declaration = 3.2;  // Stack allocation
    energy_model.assignment = 0.8;   // Load + Store
    energy_model.comparison = 0.5;   // CMP instruction
    energy_model.increment = 0.6;    // ADD instruction
    energy_model.branch = 1.2;       // Conditional branch
}

float calculate_statement_energy(Node *stmt) {
    if (!stmt) return 0;
    
    float energy = 0;
    
    if (strcmp(stmt->type, "declaration") == 0) {
        energy = energy_model.declaration;
        printf("  Declaration '%s': %.2f nJ\n", 
               stmt->var_name, energy);
    }
    else if (strcmp(stmt->type, "assignment") == 0) {
        energy = energy_model.assignment;
        printf("  Assignment '%s': %.2f nJ\n", 
               stmt->var_name, energy);
    }
    
    // Process next statement in list
    if (stmt->next) {
        energy += calculate_statement_energy(stmt->next);
    }
    
    return energy;
}

float calculate_loop_energy(Node *loop, int iterations) {
    if (strcmp(loop->type, "for_loop") != 0) {
        return 0;
    }
    
    printf("\n=== Energy Calculation ===\n");
    printf("Loop iterations: %d\n\n", iterations);
    
    // Loop initialization (one time)
    float init_energy = calculate_statement_energy(loop->init);
    printf("Init energy: %.2f nJ\n\n", init_energy);
    
    // Loop overhead per iteration
    float overhead_per_iter = 
        energy_model.comparison +  // i < n
        energy_model.increment +   // i++
        energy_model.branch;       // jump back
    
    printf("Loop overhead per iteration: %.2f nJ\n", overhead_per_iter);
    
    // Body energy per iteration
    printf("\nBody statements:\n");
    float body_energy_per_iter = calculate_statement_energy(loop->body);
    printf("Body energy per iteration: %.2f nJ\n\n", body_energy_per_iter);
    
    // Total
    float total = init_energy + 
                  (overhead_per_iter + body_energy_per_iter) * iterations;
    
    printf("TOTAL ENERGY: %.2f nJ\n", total);
    printf("========================\n\n");
    
    return total;
}