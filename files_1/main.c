/* main.c */
#include <stdio.h>
#include "ast.h"
#include "tac.h"
#include "tac_optimizer.h"
#include "energy.h"

/*
 * BUG 13 (main.c): `extern Node *root` but the parser declares
 *   `Node *root = NULL` — the extern type must match exactly.
 *   With the old dual-type system this was `extern Node *root`
 *   while tac.c expected an `ASTNode*`. Now ASTNode == Node so
 *   either name works, but we keep it as Node* to match parser.y.
 */
extern Node *root;
extern int   yyparse(void);
extern FILE *yyin;

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) { perror("fopen"); return 1; }
    }

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║         ENERGY-AWARE IoT COMPILER                         ║\n");
    printf("║     Lexer → Parser → AST → TAC → Optimize → Energy        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    /* Phase 1: Parse */
    printf("Phase 1: Lexical & Syntax Analysis\n");
    printf("═══════════════════════════════════\n");
    yyparse();
    if (!root) { fprintf(stderr, "Parsing failed\n"); return 1; }
    printf("Parsing successful\n\n");

    /* Phase 2: Print AST */
    printf("Phase 2: Abstract Syntax Tree\n");
    printf("═══════════════════════════════════\n");
    print_ast(root, 1);
    printf("\n");

    /* Phase 3: Generate TAC */
    printf("Phase 3: TAC Generation\n");
    printf("═══════════════════════════════════\n");
    TACInstruction *tac_original = generate_tac(root);
    printf("TAC generated from AST\n");
    print_tac(tac_original);

    /* Phase 4: Energy (before) */
    printf("Phase 4: Energy Analysis (Original TAC)\n");
    printf("═══════════════════════════════════\n");
    init_energy_model();
    float original_energy = calculate_tac_energy(tac_original, 100);

    /* Phase 5: Optimize TAC */
    printf("Phase 5: TAC Optimization\n");
    printf("═══════════════════════════════════\n");
    TACOptimizationResult opt_result;
    TACInstruction *tac_optimized = optimize_tac(tac_original, &opt_result);

    printf("\nOptimized TAC:\n");
    print_tac(tac_optimized);

    /* Phase 6: Energy (after) */
    printf("Phase 6: Energy Analysis (Optimized TAC)\n");
    printf("═══════════════════════════════════\n");
    float optimized_energy = calculate_tac_energy(tac_optimized, 100);

    /* Final report */
    float saved = original_energy - optimized_energy;
    float pct   = original_energy > 0
                  ? (saved / original_energy) * 100.0f : 0.0f;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf(  "║                  FINAL COMPILATION REPORT                 ║\n");
    printf(  "╠═══════════════════════════════════════════════════════════╣\n");
    printf(  "║  Original Energy:    %8.2f nJ                             ║\n", original_energy);
    printf(  "║  Optimized Energy:   %8.2f nJ                             ║\n", optimized_energy);
    printf(  "║  Energy Saved:       %8.2f nJ  (%.1f%%)                   ║\n",  saved, pct);
    printf(  "╠═══════════════════════════════════════════════════════════╣\n");
    printf(  "║  Constants folded:      %3d                               ║\n", opt_result.constants_folded);
    printf(  "║  Dead code removed:     %3d                               ║\n", opt_result.dead_code_removed);
    printf(  "║  Instructions hoisted:  %3d                               ║\n", opt_result.instructions_hoisted);
    printf(  "╚═══════════════════════════════════════════════════════════╝\n\n");

    free_tac(tac_optimized);
    free_ast(root);
    return 0;
}
