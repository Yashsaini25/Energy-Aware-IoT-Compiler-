/* main.c - Complete version */
#include <stdio.h>
#include "ast.h"
#include "energy.h"
#include "optimizer.h"

extern Node *root;
extern int yyparse();
extern FILE *yyin;

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("fopen");
            return 1;
        }
    }
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Energy-Aware IoT Compiler             ║\n");
    printf("║  Declaration Hoisting Optimizer        ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    yyparse();
    
    if (root) {
        printf("=== ORIGINAL CODE ===\n");
        print_ast(root, 0);
        
        // Calculate original energy
        init_energy_model();
        printf("\n=== BEFORE OPTIMIZATION ===\n");
        float original_energy = calculate_loop_energy(root, 100);
        
        // Optimize
        OptimizationResult result = optimize_loop(root);
        
        // Calculate optimized energy
        printf("\n=== AFTER OPTIMIZATION ===\n");
        float optimized_energy = calculate_loop_energy(root, 100);
        
        // Report
        printf("\n=== COMPARISON ===\n");
        printf("Original energy:  %.2f nJ\n", original_energy);
        printf("Optimized energy: %.2f nJ\n", optimized_energy);
        printf("Energy saved:     %.2f nJ (%.1f%%)\n\n", 
               original_energy - optimized_energy,
               ((original_energy - optimized_energy) / original_energy) * 100);
        
        print_optimization_report(result);
    }
    
    return 0;
}