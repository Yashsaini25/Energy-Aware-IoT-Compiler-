/* tac_optimizer.h - Optimization on TAC (not AST!) */
#ifndef TAC_OPTIMIZER_H
#define TAC_OPTIMIZER_H

#include "tac.h"

// Optimization statistics
typedef struct {
    int constants_folded;
    int dead_code_removed;
    int instructions_hoisted;
    float energy_saved;
} TACOptimizationResult;

// Main optimization functions
TACInstruction* optimize_tac(TACInstruction *tac, TACOptimizationResult *result);

// Individual optimization passes
TACInstruction* constant_fold_tac(TACInstruction *tac, int *folded_count);
TACInstruction* dead_code_elimination_tac(TACInstruction *tac, int *removed_count);
TACInstruction* licm_on_tac(TACInstruction *tac, int *hoisted_count);

// Helper functions
int is_constant_tac(char *operand);
int evaluate_tac_binop(TACOpcode op, int left, int right);
void print_tac_optimization_report(TACOptimizationResult result);

#endif