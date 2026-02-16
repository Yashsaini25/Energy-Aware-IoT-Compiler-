/* optimizer.h */
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ast.h"

typedef struct {
    int declarations_hoisted;
    float energy_saved;
} OptimizationResult;

OptimizationResult optimize_loop(Node *loop);
void print_optimization_report(OptimizationResult result);

#endif