#ifndef ENERGY_H
#define ENERGY_H

#include "ast.h"
#include "tac.h"

typedef struct {
    float simple_arithmetic;  // ADD, SUB (0.6 nJ)
    float multiply;           // MUL (1.5 nJ)
    float divide;             // DIV (4.0 nJ)
    float load;               // Load from memory (2.4 nJ)
    float store;              // Store to memory (2.6 nJ)
    float comparison;         // CMP (0.5 nJ)
    float branch;             // Branch (1.2 nJ)
    float assignment;         // MOV (0.8 nJ)
} EnergyModel;

extern EnergyModel energy_model;

void init_energy_model();
float calculate_tac_energy(TACInstruction *tac, int iterations);
float get_tac_instruction_energy(TACInstruction *instr);

#endif