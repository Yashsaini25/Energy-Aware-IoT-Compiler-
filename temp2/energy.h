/* energy.h */
#ifndef ENERGY_H
#define ENERGY_H

#include "ast.h"

typedef struct {
    float declaration;    // 3.2 nJ
    float assignment;     // 0.8 nJ
    float comparison;     // 0.5 nJ
    float increment;      // 0.6 nJ
    float branch;         // 1.2 nJ
} EnergyModel;

extern EnergyModel energy_model;

void init_energy_model();
float calculate_loop_energy(Node *loop, int iterations);
float calculate_statement_energy(Node *stmt);

#endif