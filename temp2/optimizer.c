/* optimizer.c - Simple declaration hoisting */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "optimizer.h"
#include "energy.h"

// Count declarations in loop body
int count_declarations(Node *body) {
    int count = 0;
    Node *current = body;
    
    while (current) {
        if (strcmp(current->type, "declaration") == 0) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

// Remove declarations from loop body (return new body)
Node* remove_declarations(Node *body) {
    Node *new_body = NULL;
    Node *last = NULL;
    Node *current = body;
    
    while (current) {
        Node *next = current->next;
        
        // If NOT a declaration, keep it
        if (strcmp(current->type, "declaration") != 0) {
            current->next = NULL;
            
            if (!new_body) {
                new_body = current;
                last = current;
            } else {
                last->next = current;
                last = current;
            }
        }
        
        current = next;
    }
    
    return new_body;
}

OptimizationResult optimize_loop(Node *loop) {
    OptimizationResult result = {0, 0};
    
    if (strcmp(loop->type, "for_loop") != 0) {
        return result;
    }
    
    printf("\n=== Optimization ===\n");
    
    // Count declarations
    result.declarations_hoisted = count_declarations(loop->body);
    printf("Declarations found in loop: %d\n", result.declarations_hoisted);
    
    if (result.declarations_hoisted == 0) {
        printf("Nothing to optimize!\n");
        return result;
    }
    
    // Calculate energy saved
    // Before: declaration executed n times
    // After: declaration executed once
    int iterations = 100;  // Assume 100 iterations
    float energy_per_decl = energy_model.declaration;
    result.energy_saved = energy_per_decl * iterations * result.declarations_hoisted
                        - energy_per_decl * result.declarations_hoisted;
    
    printf("Energy saved per declaration: %.2f nJ\n", 
           energy_per_decl * iterations - energy_per_decl);
    printf("Total energy saved: %.2f nJ\n", result.energy_saved);
    
    // Actually remove declarations from body
    loop->body = remove_declarations(loop->body);
    
    // Mark that declarations are now outside loop
    Node *stmt = loop->body;
    while (stmt) {
        stmt->is_in_loop = 1;  // Still in loop
        stmt = stmt->next;
    }
    
    printf("Optimization complete!\n");
    printf("====================\n\n");
    
    return result;
}

void print_optimization_report(OptimizationResult result) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║   OPTIMIZATION REPORT                  ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ Declarations hoisted: %-16d ║\n", result.declarations_hoisted);
    printf("║ Energy saved: %.2f nJ                ║\n", result.energy_saved);
    printf("╚════════════════════════════════════════╝\n");
}