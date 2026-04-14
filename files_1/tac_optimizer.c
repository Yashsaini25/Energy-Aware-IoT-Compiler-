/* tac_optimizer.c - TAC Optimization */
#include "tac_optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ══════════════════════════════════════════════════════
   HELPERS
   ══════════════════════════════════════════════════════ */

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

int evaluate_tac_binop(TACOpcode op, int left, int right) {
    switch (op) {
        case TAC_ADD: return left + right;
        case TAC_SUB: return left - right;
        case TAC_MUL: return left * right;
        case TAC_DIV: return right != 0 ? left / right : 0;
        case TAC_LT:  return left <  right;
        case TAC_GT:  return left >  right;
        case TAC_LE:  return left <= right;
        case TAC_GE:  return left >= right;
        case TAC_EQ:  return left == right;
        case TAC_NE:  return left != right;
        default:      return 0;
    }
}

/* ══════════════════════════════════════════════════════
   PASS 1 — DEAD CODE ELIMINATION  (liveness-based)

   Why the old version was wrong:
   ───────────────────────────────
   The old DCE just scanned the instruction list looking
   for whether cur->result appeared as arg1/arg2 in any
   other instruction.  It had no model of control flow,
   so it never knew that  i = t1  (inside the loop) feeds
   back into  t0 = i < n  (at the loop header) via the
   back-edge  goto L0.  With constant folding also running
   before it, i/t1 got replaced with literals, making them
   look completely unused — so everything was deleted.

   Correct algorithm — iterative backward liveness:
   ─────────────────────────────────────────────────
   1. Linearise the linked list into an index array.
   2. For each instruction record:
        uses[i]  = variables READ   (arg1, arg2)
        defs[i]  = variables WRITTEN (result, for non-jumps)
   3. Solve backward:
        live_out[i] = ∪ live_in[j]  for every successor j
        live_in[i]  = uses[i] ∪ (live_out[i] − defs[i])
      Successors respect GOTO / IF_FALSE_GOTO so the loop
      back-edge is properly handled.
   4. An instruction is dead iff it writes a variable AND
      that variable is not in live_out[i], AND the op has
      no side effects.
   5. Remove one dead instruction, then restart (keeps the
      logic simple; the TAC list is always short).
   ══════════════════════════════════════════════════════ */

#define MAX_VARS   64
#define MAX_INSTRS 512

typedef struct {
    char *v[MAX_VARS];
    int   n;
} VarSet;

static void vs_clear(VarSet *s) { s->n = 0; }

static int vs_contains(const VarSet *s, const char *name) {
    if (!name) return 0;
    for (int i = 0; i < s->n; i++)
        if (strcmp(s->v[i], name) == 0) return 1;
    return 0;
}

/* Only track genuine variable names; skip integer literals */
static void vs_add(VarSet *s, const char *name) {
    if (!name || *name == '\0') return;
    const char *p = name;
    if (*p == '-' || *p == '+') p++;
    int all_digits = (*p != '\0');
    for (const char *q = p; *q; q++)
        if (!isdigit((unsigned char)*q)) { all_digits = 0; break; }
    if (all_digits) return;          /* skip "0", "1", "5", etc. */
    if (vs_contains(s, name)) return;
    if (s->n < MAX_VARS) s->v[s->n++] = (char *)name;
}

static void vs_union(VarSet *dst, const VarSet *src) {
    for (int i = 0; i < src->n; i++) vs_add(dst, src->v[i]);
}

static int vs_equal(const VarSet *a, const VarSet *b) {
    if (a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++)
        if (!vs_contains(b, a->v[i])) return 0;
    return 1;
}

static int find_label_idx(TACInstruction **arr, int n, const char *label) {
    if (!label) return -1;
    for (int i = 0; i < n; i++)
        if (arr[i]->op == TAC_LABEL && arr[i]->result &&
            strcmp(arr[i]->result, label) == 0)
            return i;
    return -1;
}

TACInstruction* dead_code_elimination_tac(TACInstruction *tac,
                                          int *removed_count) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║   DEAD CODE ELIMINATION (on TAC)       ║\n");
    printf(  "╚════════════════════════════════════════╝\n\n");

    int any_removed = 1;
    while (any_removed) {
        any_removed = 0;

        /* Step 1: linearise */
        TACInstruction *arr[MAX_INSTRS];
        int n = 0;
        for (TACInstruction *p = tac; p && n < MAX_INSTRS; p = p->next)
            arr[n++] = p;
        if (n == 0) break;

        /* Step 2: USE / DEF sets */
        VarSet uses[MAX_INSTRS], defs[MAX_INSTRS];
        for (int i = 0; i < n; i++) {
            vs_clear(&uses[i]); vs_clear(&defs[i]);
            TACInstruction *ins = arr[i];

            vs_add(&uses[i], ins->arg1);
            vs_add(&uses[i], ins->arg2);

            if (ins->result &&
                ins->op != TAC_LABEL &&
                ins->op != TAC_GOTO  &&
                ins->op != TAC_IF_FALSE_GOTO &&
                ins->op != TAC_IF_TRUE_GOTO)
                vs_add(&defs[i], ins->result);
        }

        /* Step 3: iterative backward liveness */
        VarSet live_in[MAX_INSTRS], live_out[MAX_INSTRS];
        for (int i = 0; i < n; i++) {
            vs_clear(&live_in[i]);
            vs_clear(&live_out[i]);
        }

        int changed = 1;
        while (changed) {
            changed = 0;
            for (int i = n - 1; i >= 0; i--) {
                TACInstruction *ins = arr[i];
                VarSet new_out; vs_clear(&new_out);

                /* fall-through successor */
                if (ins->op != TAC_GOTO && i + 1 < n)
                    vs_union(&new_out, &live_in[i + 1]);

                /* jump-target successor */
                if (ins->op == TAC_GOTO ||
                    ins->op == TAC_IF_FALSE_GOTO ||
                    ins->op == TAC_IF_TRUE_GOTO) {
                    int tgt = find_label_idx(arr, n, ins->result);
                    if (tgt >= 0) vs_union(&new_out, &live_in[tgt]);
                }

                /* live_in = uses ∪ (live_out − defs) */
                VarSet new_in; vs_clear(&new_in);
                vs_union(&new_in, &uses[i]);
                for (int k = 0; k < new_out.n; k++)
                    if (!vs_contains(&defs[i], new_out.v[k]))
                        vs_add(&new_in, new_out.v[k]);

                if (!vs_equal(&new_in,  &live_in[i]) ||
                    !vs_equal(&new_out, &live_out[i])) {
                    live_in[i]  = new_in;
                    live_out[i] = new_out;
                    changed = 1;
                }
            }
        }

        /* Step 4: remove first dead instruction found */
        for (int i = 0; i < n; i++) {
            TACInstruction *ins = arr[i];

            if (defs[i].n == 0) continue;                          /* no def */
            if (ins->op == TAC_CALL || ins->op == TAC_RETURN) continue; /* side-effect */

            int still_live = 0;
            for (int d = 0; d < defs[i].n; d++)
                if (vs_contains(&live_out[i], defs[i].v[d]))
                    { still_live = 1; break; }

            if (!still_live) {
                printf("  Removing dead code: %s = ...\n", ins->result);
                (*removed_count)++;
                any_removed = 1;

                if (i == 0) tac = ins->next;
                else        arr[i-1]->next = ins->next;

                free(ins->result); free(ins->arg1); free(ins->arg2);
                free(ins);
                break;   /* restart with fresh liveness */
            }
        }
    }

    printf("Total dead code removed: %d\n", *removed_count);
    printf("════════════════════════════════════════\n\n");
    return tac;
}

/* ══════════════════════════════════════════════════════
   PASS 2 — LOOP-INVARIANT CODE MOTION (LICM)
   ══════════════════════════════════════════════════════ */

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

    /* find first IF_FALSE_GOTO */
    for (TACInstruction *i = tac; i; i = i->next)
        if (i->op == TAC_IF_FALSE_GOTO) { exit_jump = i; break; }

    if (!exit_jump) {
        printf("  No loop found in TAC.\n");
        printf("════════════════════════════════════════\n\n");
        return tac;
    }

    /* find the loop-start label (last TAC_LABEL before exit_jump) */
    for (TACInstruction *i = tac; i && i != exit_jump; i = i->next)
        if (i->op == TAC_LABEL) loop_label = i;

    /* find back-edge GOTO targeting loop_label */
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

    /* identify induction variable: pattern  tX = V + 1 ; V = tX */
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

    /* hoist loop-invariant TAC_ASSIGN instructions */
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

            int invariant = is_constant_tac(cur->arg1) ||
                            !is_written_in_range(exit_jump->next,
                                                 back_edge, cur->arg1);
            if (!invariant) { prev = cur; cur = next; continue; }

            printf("  Hoisting: %s = %s  (moved before loop)\n",
                   cur->result, cur->arg1);
            (*hoisted_count)++;
            changed = 1;

            /* unlink cur */
            if (prev) prev->next = next;
            else      exit_jump->next = next;
            cur->next = NULL;

            /* insert immediately before loop_label */
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

/* ══════════════════════════════════════════════════════
   MAIN ENTRY — DCE then LICM  (constant folding removed)
   ══════════════════════════════════════════════════════ */
TACInstruction* optimize_tac(TACInstruction *tac,
                              TACOptimizationResult *result) {
    result->constants_folded      = 0;
    result->dead_code_removed     = 0;
    result->instructions_hoisted  = 0;
    result->energy_saved          = 0;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf(  "║              TAC OPTIMIZATION PASSES                      ║\n");
    printf(  "╚═══════════════════════════════════════════════════════════╝\n");

    tac = dead_code_elimination_tac(tac, &result->dead_code_removed);
    tac = licm_on_tac(tac,               &result->instructions_hoisted);

    result->energy_saved =
        result->dead_code_removed    * 0.8f +
        result->instructions_hoisted * 3.2f;

    return tac;
}

void print_tac_optimization_report(TACOptimizationResult result) {
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf(  "║           TAC OPTIMIZATION REPORT                         ║\n");
    printf(  "╠═══════════════════════════════════════════════════════════╣\n");
    printf(  "║  Dead code removed:       %-3d                            ║\n", result.dead_code_removed);
    printf(  "║  Instructions hoisted:    %-3d                            ║\n", result.instructions_hoisted);
    printf(  "║  Energy saved (approx):   %.2f nJ                        ║\n", result.energy_saved);
    printf(  "╚═══════════════════════════════════════════════════════════╝\n\n");
}
