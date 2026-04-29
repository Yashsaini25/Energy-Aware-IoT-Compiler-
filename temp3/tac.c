#include "tac.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TACGenerator* create_tac_generator(void) {
    TACGenerator *gen = calloc(1, sizeof(TACGenerator));
    if (!gen) { fprintf(stderr, "OOM\n"); exit(1); }
    return gen;
}

char* new_temp(TACGenerator *gen) {
    char *t = malloc(20);
    sprintf(t, "t%d", gen->temp_count++);
    return t;
}

char* new_label(TACGenerator *gen) {
    char *l = malloc(20);
    sprintf(l, "L%d", gen->label_count++);
    return l;
}

TACInstruction* create_tac_instr(TACOpcode op, char *result, char *arg1, char *arg2) {
    TACInstruction *i = calloc(1, sizeof(TACInstruction));
    if (!i) { fprintf(stderr, "OOM\n"); exit(1); }
    i->op     = op;
    i->result = result ? strdup(result) : NULL;
    i->arg1   = arg1   ? strdup(arg1)   : NULL;
    i->arg2   = arg2   ? strdup(arg2)   : NULL;
    return i;
}

void append_tac(TACGenerator *gen, TACInstruction *instr) {
    if (!gen->head) gen->head = gen->tail = instr;
    else { gen->tail->next = instr; gen->tail = instr; }
}

static char* gen_binop(ASTNode* node, TACGenerator* gen){
    char* left = generate_tac_for_node(node->left, gen);
    char* right = generate_tac_for_node(node->right, gen);
    char* temp = new_temp(gen);

    TACOpcode op;

    if(node->op_str){
        if      (strcmp(node->op_str, "+") == 0) op = TAC_ADD;
        else if (strcmp(node->op_str, "-") == 0) op = TAC_SUB;
        else if (strcmp(node->op_str, "*") == 0) op = TAC_MUL;
        else if (strcmp(node->op_str, "/") == 0) op = TAC_DIV;
        else if (strcmp(node->op_str, "==") == 0) op = TAC_EQ;
        else if (strcmp(node->op_str, "!=") == 0) op = TAC_NE;
        else if (strcmp(node->op_str, "<") == 0) op = TAC_LT;
        else if (strcmp(node->op_str, ">") == 0) op = TAC_GT;
        else if (strcmp(node->op_str, "<=") == 0) op = TAC_LE;
        else if (strcmp(node->op_str, ">=") == 0) op = TAC_GE;
    }

    append_tac(gen, create_tac_instr(op, temp, left, right));
    free(left);
    free(right);
    return temp;
}

static char* gen_identifier(ASTNode *node, TACGenerator *gen) {
    (void)gen;
    return strdup(node->name ? node->name : "?");  /* BUG 8 FIX */
}

static char* gen_constant(ASTNode* node, TACGenerator* gen){
    char* v = malloc(20);
    sprintf(v, "%d", node->int_value);
    return v;
}

static char* gen_assignment(ASTNode* node, TACGenerator* gen){
    char* rhs;

    if(node->initializer){
        rhs = generate_tac_for_node(node->initializer, gen);
    } else {
        rhs = malloc(20);
        sprintf(rhs, "%d", node->init_value);
    }

    append_tac(gen, create_tac_instr(TAC_ASSIGN, node->var_name, rhs, NULL));
    free(rhs);
    return node->var_name;
}

static char* gen_declaration(ASTNode* node, TACGenerator* gen){
    char* rhs = NULL;

    if(node->initializer){
        rhs = generate_tac_for_node(node->initializer, gen);
    }
    else if(node->has_init){
        rhs = malloc(20);
        sprintf(rhs, "%d", node->init_value);
    }

    if(rhs){
        append_tac(gen, create_tac_instr(TAC_ASSIGN, node->var_name, rhs, NULL));
        free(rhs);
    }
    return node->var_name;
}

static char* gen_for_loop(ASTNode* node, TACGenerator* gen){
    generate_tac_for_node(node->init, gen);

    char* lstart = new_label(gen);
    append_tac(gen, create_tac_instr(TAC_LABEL, lstart, NULL, NULL));

    char* ctemp = new_temp(gen);
    append_tac(gen, create_tac_instr(TAC_LT, ctemp, node->cond_var, node->cond_limit));

    char* lend = new_label(gen);
    append_tac(gen, create_tac_instr(TAC_IF_FALSE_GOTO, lend, ctemp, NULL));

    ASTNode* stmt = node->body;
    while(stmt){
        generate_tac_for_node(stmt, gen);
        stmt = stmt->next;
    }

    char* inc = new_temp(gen);
    append_tac(gen, create_tac_instr(TAC_ADD, inc, node->incr_var, "1"));
    append_tac(gen, create_tac_instr(TAC_ASSIGN, node->incr_var, inc, NULL));

    append_tac(gen, create_tac_instr(TAC_GOTO, lstart, NULL, NULL));
    append_tac(gen, create_tac_instr(TAC_LABEL, lend, NULL, NULL));

    free(lstart);
    free(lend);
    free(ctemp);
    free(inc);
    return NULL;
}

char* generate_tac_for_node(ASTNode* node, TACGenerator* gen){
    if(!node) return NULL;

    switch(node->type){
        case NODE_CONSTANT: return gen_constant(node, gen);
        case NODE_IDENTIFIER: return gen_identifier(node, gen);
        case NODE_BINARY_OP: return gen_binop(node, gen);
        case NODE_ASSIGNMENT: return gen_assignment(node, gen);
        case NODE_DECLARATION: return gen_declaration(node, gen);
        case NODE_FOR_LOOP: return gen_for_loop(node, gen);
        case NODE_PROGRAM: {
            ASTNode* stmt = node->statements;
            while(stmt){
                generate_tac_for_node(stmt, gen);
                stmt = stmt->next;
            }
            return NULL;
        }
        default: return NULL;
    }
}

TACInstruction* generate_tac(ASTNode* ast){
    TACGenerator* gen = create_tac_generator();
    generate_tac_for_node(ast, gen);
    return gen->head;
}

void print_tac(TACInstruction *tac) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║   THREE-ADDRESS CODE (TAC)             ║\n");
    printf(  "╚════════════════════════════════════════╝\n\n");
    int count = 0;
    while (tac) {
        switch (tac->op) {
            case TAC_ASSIGN:       printf("  %s = %s\n",           tac->result, tac->arg1); break;
            case TAC_ADD:          printf("  %s = %s + %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_SUB:          printf("  %s = %s - %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_MUL:          printf("  %s = %s * %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_DIV:          printf("  %s = %s / %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_LT:           printf("  %s = %s < %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_GT:           printf("  %s = %s > %s\n",      tac->result, tac->arg1, tac->arg2); break;
            case TAC_LE:           printf("  %s = %s <= %s\n",     tac->result, tac->arg1, tac->arg2); break;
            case TAC_GE:           printf("  %s = %s >= %s\n",     tac->result, tac->arg1, tac->arg2); break;
            case TAC_EQ:           printf("  %s = %s == %s\n",     tac->result, tac->arg1, tac->arg2); break;
            case TAC_NE:           printf("  %s = %s != %s\n",     tac->result, tac->arg1, tac->arg2); break;
            case TAC_LABEL:        printf("%s:\n",                  tac->result); break;
            case TAC_GOTO:         printf("  goto %s\n",            tac->result); break;
            case TAC_IF_FALSE_GOTO:printf("  if_false %s goto %s\n",tac->arg1, tac->result); break;
            case TAC_IF_TRUE_GOTO: printf("  if_true %s goto %s\n", tac->arg1, tac->result); break;
            case TAC_PARAM:        printf("  param %s\n",           tac->arg1); break;
            case TAC_CALL:         printf("  %s = call %s, %s\n",  tac->result, tac->arg1, tac->arg2); break;
            case TAC_RETURN:       printf("  return%s%s\n",         tac->arg1 ? " " : "", tac->arg1 ? tac->arg1 : ""); break;
            default:               printf("  <unknown op: %d>\n",   tac->op);
        }
        count++;
        tac = tac->next;
    }
    printf("\nTotal TAC instructions: %d\n", count);
    printf("════════════════════════════════════════\n\n");
}

void free_tac(TACInstruction *tac) {
    while (tac) {
        TACInstruction *next = tac->next;
        free(tac->result); free(tac->arg1); free(tac->arg2);
        free(tac);
        tac = next;
    }
}