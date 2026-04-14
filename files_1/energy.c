/* energy.c - Energy calculation from TAC */
#include "energy.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

EnergyModel energy_model;

void init_energy_model() {
    energy_model.simple_arithmetic = 0.6f;
    energy_model.multiply          = 1.5f;
    energy_model.divide            = 4.0f;
    energy_model.load              = 2.4f;
    energy_model.store             = 2.6f;
    energy_model.comparison        = 0.5f;
    energy_model.branch            = 1.2f;
    energy_model.assignment        = 0.8f;
}

float get_tac_instruction_energy(TACInstruction *instr) {
    if (!instr) return 0.0f;
    switch (instr->op) {
        case TAC_ADD:
        case TAC_SUB:
            return energy_model.simple_arithmetic
                 + energy_model.load * 2
                 + energy_model.store;
        case TAC_MUL:
            return energy_model.multiply
                 + energy_model.load * 2
                 + energy_model.store;
        case TAC_DIV:
            return energy_model.divide
                 + energy_model.load * 2
                 + energy_model.store;
        case TAC_ASSIGN:
            return energy_model.assignment
                 + energy_model.load
                 + energy_model.store;
        case TAC_LT: case TAC_GT:
        case TAC_LE: case TAC_GE:
        case TAC_EQ: case TAC_NE:
            return energy_model.comparison
                 + energy_model.load * 2;
        case TAC_GOTO:
            return energy_model.branch;
        case TAC_IF_FALSE_GOTO:
        case TAC_IF_TRUE_GOTO:
            return energy_model.comparison + energy_model.branch;
        case TAC_LABEL:
            return 0.0f;
        case TAC_PARAM:
            return energy_model.store;
        case TAC_CALL:
            return energy_model.branch * 2;
        case TAC_RETURN:
            return energy_model.branch;
        default:
            return 1.0f;
    }
}

/*
 * Calculate total energy from a TAC list.
 *
 * Strategy:
 *   - Instructions OUTSIDE the loop are counted once.
 *   - Instructions INSIDE the loop are counted `iterations` times.
 *
 * Loop boundaries are detected by finding the first TAC_IF_FALSE_GOTO
 * (loop entry test) and the TAC_GOTO that jumps back (back edge).
 * Everything between those two markers is the loop body.
 */
float calculate_tac_energy(TACInstruction *tac, int iterations) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║   ENERGY ANALYSIS FROM TAC             ║\n");
    printf(  "╚════════════════════════════════════════╝\n\n");

    if (!tac) {
        printf("  (empty TAC)\n");
        return 0.0f;
    }

    /* ── Locate loop boundaries ── */
    TACInstruction *exit_jump = NULL;   /* if_false … goto Lend */
    TACInstruction *back_edge = NULL;   /* goto Lstart          */

    /* Find exit_jump (first IF_FALSE_GOTO) */
    for (TACInstruction *i = tac; i; i = i->next) {
        if (i->op == TAC_IF_FALSE_GOTO) { exit_jump = i; break; }
    }

    /* Find back_edge: GOTO that targets the label just before exit_jump */
    if (exit_jump) {
        /* The loop start label is the one whose ->next leads to exit_jump
           (possibly with a condition temp in between).  We identify it by
           finding the TAC_GOTO whose result matches that label name.      */
        /* Simpler: find the LAST TAC_GOTO before the end label */
        TACInstruction *last_goto = NULL;
        for (TACInstruction *i = exit_jump; i; i = i->next) {
            if (i->op == TAC_GOTO) last_goto = i;
        }
        back_edge = last_goto;
    }

    float total        = 0.0f;
    float loop_body_eu = 0.0f;
    float outside_eu   = 0.0f;
    int   body_count   = 0;
    int   outside_count= 0;

    printf("%-30s %10s  %s\n", "Instruction", "EU/exec", "Region");
    printf("──────────────────────────────────────────────────\n");

    int in_loop = 0;
    for (TACInstruction *cur = tac; cur; cur = cur->next) {

        /* Transition into loop body just after exit_jump */
        if (cur == exit_jump->next && exit_jump) in_loop = 1;

        /* Transition out of loop body at back_edge (inclusive — back_edge
           itself is the goto, which still runs every iteration) */
        if (cur == back_edge && back_edge) {
            /* count back_edge this pass, then leave loop */
        }

        float eu = get_tac_instruction_energy(cur);

        /* Print a readable description */
        char desc[64] = {0};
        switch (cur->op) {
            case TAC_ASSIGN:
                snprintf(desc, sizeof(desc), "%s = %s",
                         cur->result ? cur->result : "?",
                         cur->arg1   ? cur->arg1   : "?");
                break;
            case TAC_ADD:
                snprintf(desc, sizeof(desc), "%s = %s + %s",
                         cur->result, cur->arg1, cur->arg2);
                break;
            case TAC_SUB:
                snprintf(desc, sizeof(desc), "%s = %s - %s",
                         cur->result, cur->arg1, cur->arg2);
                break;
            case TAC_MUL:
                snprintf(desc, sizeof(desc), "%s = %s * %s",
                         cur->result, cur->arg1, cur->arg2);
                break;
            case TAC_DIV:
                snprintf(desc, sizeof(desc), "%s = %s / %s",
                         cur->result, cur->arg1, cur->arg2);
                break;
            case TAC_LT:
                snprintf(desc, sizeof(desc), "%s = %s < %s",
                         cur->result, cur->arg1, cur->arg2);
                break;
            case TAC_LABEL:
                snprintf(desc, sizeof(desc), "%s:", cur->result);
                break;
            case TAC_GOTO:
                snprintf(desc, sizeof(desc), "goto %s", cur->result);
                break;
            case TAC_IF_FALSE_GOTO:
                snprintf(desc, sizeof(desc), "if_false %s goto %s",
                         cur->arg1, cur->result);
                break;
            default:
                snprintf(desc, sizeof(desc), "(op=%d)", cur->op);
        }

        const char *region = in_loop ? "LOOP BODY" : "outside";
        printf("  %-28s  %8.2f  %s\n", desc, eu, region);

        if (in_loop) {
            loop_body_eu += eu;
            body_count++;
        } else {
            outside_eu += eu;
            outside_count++;
        }

        /* Leave loop region after back_edge */
        if (cur == back_edge) in_loop = 0;
    }

    /*
     * Total = outside instructions (once) +
     *         loop body instructions × iterations +
     *         loop condition check × iterations  (already inside loop_body_eu
     *         because exit_jump is included in the in_loop range)
     */
    total = outside_eu + loop_body_eu * iterations;

    printf("\n──────────────────────────────────────────────────\n");
    printf("Outside-loop instructions : %d  (%.2f EU, run once)\n",
           outside_count, outside_eu);
    printf("Inside-loop  instructions : %d  (%.2f EU × %d iters)\n",
           body_count, loop_body_eu, iterations);
    printf("TOTAL ENERGY              : %.2f EU\n", total);
    printf("════════════════════════════════════════\n\n");

    return total;
}
