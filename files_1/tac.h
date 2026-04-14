/* tac.h - Three-Address Code */
#ifndef TAC_H
#define TAC_H

#include "ast.h"

typedef enum {
    TAC_ASSIGN,
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV,
    TAC_LT, TAC_GT, TAC_LE, TAC_GE, TAC_EQ, TAC_NE,
    TAC_LABEL,
    TAC_GOTO,
    TAC_IF_FALSE_GOTO,
    TAC_IF_TRUE_GOTO,
    TAC_PARAM,
    TAC_CALL,
    TAC_RETURN
} TACOpcode;

typedef struct TACInstruction {
    TACOpcode op;
    char *result;
    char *arg1;
    char *arg2;
    int   line;
    struct TACInstruction *next;
} TACInstruction;

typedef struct {
    int temp_count;
    int label_count;
    TACInstruction *head;
    TACInstruction *tail;
} TACGenerator;

TACGenerator*    create_tac_generator(void);
TACInstruction*  generate_tac(ASTNode *ast);

/*
 * BUG 6 (tac.h): generate_tac_for_node was declared to return char*
 *   but the for-loop case always returned NULL — the return type was
 *   correct but callers that used the result for compound/program
 *   nodes discarded it anyway.  No change needed here, but the
 *   implementation in tac.c is cleaned up.
 */
char* generate_tac_for_node(ASTNode *node, TACGenerator *gen);

void  print_tac(TACInstruction *tac);
void  free_tac(TACInstruction *tac);

char* new_temp(TACGenerator *gen);
char* new_label(TACGenerator *gen);
void  append_tac(TACGenerator *gen, TACInstruction *instr);
TACInstruction* create_tac_instr(TACOpcode op, char *result,
                                 char *arg1, char *arg2);

#endif
