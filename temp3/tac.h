#ifndef TAC_H
#define TAC_H

#include "ast.h"

typedef enum{
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
}TACOpcode;

typedef struct TACInstruction{
    TACOpcode op;
    char* result;
    char* arg1; 
    char* arg2;
    int line;
    struct TACInstruction* next;
}TACInstruction;

typedef struct{
    int temp_count;
    int label_count;
    TACInstruction* head;
    TACInstruction* tail;
}TACGenerator;


TACGenerator* create_tac_generator(void);
TACInstruction* generate_tac(ASTNode* ast);
TACInstruction* create_tac_instr(TACOpcode op, char* result, char* arg1, char* arg2);

char* generate_tac_for_node(ASTNode* node, TACGenerator* gen);
char* new_temp(TACGenerator* gen);
char* new_label(TACGenerator* gen);
void append_tac(TACGenerator* gen, TACInstruction* instr);

void  print_tac(TACInstruction *tac);
void  free_tac(TACInstruction *tac);

#endif