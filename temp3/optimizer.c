/* tac_optimizer.c - TAC Optimization */
#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int is_constant_tac(char *operand) {
    if (!operand || *operand == '\0') return 0;
    char *p = operand;
    if (*p == '-' || *p == '+') p++;
    if (*p == '\0') return 0;
    while (*p) {
        if (!isdigit((unsigned char)*p)) return 0;
        p++;
    }
    return 1;
}               

static int is_written_in_range(TACInstruction *from, TACInstruction *to,
                                const char *name) {
    for (TACInstruction *i = from; i && i != to; i = i->next)
        if (i->result && strcmp(i->result, name) == 0) return 1;
    return 0;
}

TACInstruction* licm_on_tac(TACInstruction *tac, int *hoisted_count) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║   LICM — Loop-Invariant Code Motion    ║\n");
    printf(  "╚════════════════════════════════════════╝\n\n");

    TACInstruction *loop_label = NULL;
    TACInstruction *exit_jump  = NULL;
    TACInstruction *back_edge  = NULL;
    TACInstruction *end_label  = NULL;

    for (TACInstruction *i = tac; i; i = i->next)
        if (i->op == TAC_IF_FALSE_GOTO) { exit_jump = i; break; }

    if (!exit_jump) {
        printf("  No loop found in TAC.\n");
        printf("════════════════════════════════════════\n\n");
        return tac;
    }

    for (TACInstruction *i = tac; i && i != exit_jump; i = i->next)
        if (i->op == TAC_LABEL) loop_label = i;

    if (loop_label) {
        for (TACInstruction *i = exit_jump; i; i = i->next)
            if (i->op == TAC_GOTO && i->result && loop_label->result &&
                strcmp(i->result, loop_label->result) == 0)
                { back_edge = i; break; }
    }

    if (back_edge) end_label = back_edge->next;

    if (!loop_label || !back_edge) {
        printf("  Could not identify full loop structure.\n");
        printf("════════════════════════════════════════\n\n");
        return tac;
    }

    printf("  Loop: %s → %s\n\n",
           loop_label->result ? loop_label->result : "?",
           end_label && end_label->result ? end_label->result : "end");

    char *induction_var = NULL;
    for (TACInstruction *i = exit_jump->next; i && i != back_edge; i = i->next)
        if (i->op == TAC_ADD && i->arg2 && strcmp(i->arg2, "1") == 0 &&
            i->next && i->next->op == TAC_ASSIGN &&
            i->next->arg1 && strcmp(i->next->arg1, i->result) == 0) {
            induction_var = strdup(i->next->result);
            break;
        }

    printf("  Induction variable: %s\n\n",
           induction_var ? induction_var : "(unknown)");

    int changed = 1;
    while (changed) {
        changed = 0;
        TACInstruction *prev = NULL;
        TACInstruction *cur  = exit_jump->next;

        while (cur && cur != back_edge) {
            TACInstruction *next = cur->next;

            if (cur->op != TAC_ASSIGN) { prev = cur; cur = next; continue; }

            if (induction_var && cur->result &&
                strcmp(cur->result, induction_var) == 0)
                { prev = cur; cur = next; continue; }

            int lhs_write_count = 0;
            for (TACInstruction *s = exit_jump->next; s && s != back_edge; s = s->next){
                if (s->result && strcmp(s->result, cur->result) == 0)
                lhs_write_count++;
            }

            int rhs_written = 0;
            if (!is_constant_tac(cur->arg1))  
            rhs_written = is_written_in_range(exit_jump->next, back_edge, cur->arg1);

            int invariant = (lhs_write_count == 1) && !rhs_written;

            if (!invariant) { prev = cur; cur = next; continue; }

            printf("  Hoisting: %s = %s  (moved before loop)\n",
                   cur->result, cur->arg1);
            (*hoisted_count)++;
            changed = 1;

            if (prev) prev->next = next;
            else      exit_jump->next = next;
            cur->next = NULL;

            if (tac == loop_label) {
                cur->next = tac; tac = cur;
            } else {
                TACInstruction *p = tac;
                while (p->next && p->next != loop_label) p = p->next;
                cur->next = p->next;
                p->next   = cur;
            }
            cur = next;
        }
    }

    if (induction_var) free(induction_var);
    printf("\nTotal instructions hoisted: %d\n", *hoisted_count);
    printf("════════════════════════════════════════\n\n");
    return tac;
}

TACInstruction* optimize_tac(TACInstruction *tac,
                              TACOptimizationResult *result) {
    result->instructions_hoisted  = 0;
    result->energy_saved          = 0;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf(  "║              TAC OPTIMIZATION PASSES                      ║\n");
    printf(  "╚═══════════════════════════════════════════════════════════╝\n");

    tac = licm_on_tac(tac,               &result->instructions_hoisted);

    result->energy_saved = result->instructions_hoisted * 3.2f;

    return tac;
}

void print_tac_optimization_report(TACOptimizationResult result) {
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf(  "║           TAC OPTIMIZATION REPORT                         ║\n");
    printf(  "╠═══════════════════════════════════════════════════════════╣\n");
    printf(  "║  Instructions hoisted:    %-3d                            ║\n", result.instructions_hoisted);
    printf(  "║  Energy saved (approx):   %.2f nJ                        ║\n", result.energy_saved);
    printf(  "╚═══════════════════════════════════════════════════════════╝\n\n");
}
