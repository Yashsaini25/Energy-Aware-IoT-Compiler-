/* tac_optimizer.h - Optimization on TAC (not AST!) */
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tac.h"

typedef struct {
    int instructions_hoisted;
    float energy_saved;
} TACOptimizationResult;

TACInstruction* optimize_tac(TACInstruction *tac, TACOptimizationResult *result);
TACInstruction* licm_on_tac(TACInstruction *tac, int *hoisted_count);

int is_constant_tac(char *operand);
int evaluate_tac_binop(TACOpcode op, int left, int right);
void print_tac_optimization_report(TACOptimizationResult result);

#endif