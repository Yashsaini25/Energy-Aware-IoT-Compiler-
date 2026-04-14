/* ast.c - AST implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ── helper: allocate zeroed node ── */
static Node* alloc_node(NodeType type) {
    Node *n = calloc(1, sizeof(Node));
    if (!n) { fprintf(stderr, "OOM\n"); exit(1); }
    n->type = type;
    return n;
}

/* ─────────────────────────────────────────────────────────────
 * BUG 4 (ast.c): make_for_loop set n->type = "for_loop" (string).
 *   With the new enum system this must be NODE_FOR_LOOP.
 * BUG 5 (ast.c): make_assignment_var never stored the source
 *   variable name — the rhs `var` argument was silently dropped.
 * ───────────────────────────────────────────────────────────── */

Node* make_for_loop(Node *init, char *cond_var, char *limit,
                    char *incr, Node *body) {
    Node *n      = alloc_node(NODE_FOR_LOOP);   /* was: "for_loop" string */
    n->init      = init;
    n->cond_var  = strdup(cond_var);
    n->cond_limit= strdup(limit);
    n->incr_var  = strdup(incr);
    n->body      = body;
    mark_nodes_in_loop(body);
    return n;
}

Node* make_declaration(char *name) {
    Node *n    = alloc_node(NODE_DECLARATION);  /* was: "declaration" */
    n->var_name= strdup(name);
    return n;
}

Node* make_declaration_with_init(char *name, int value) {
    Node *n       = alloc_node(NODE_DECLARATION);
    n->var_name   = strdup(name);
    n->has_init   = 1;
    n->init_value = value;
    return n;
}

Node* make_assignment(char *name, int value) {
    Node *n       = alloc_node(NODE_ASSIGNMENT);  /* was: "assignment" */
    n->var_name   = strdup(name);
    n->init_value = value;
    return n;
}

Node* make_assignment_var(char *name, char *var) {
    Node *n     = alloc_node(NODE_ASSIGNMENT);
    n->var_name = strdup(name);
    /* BUG 5 FIX: store the source variable as an identifier child */
    n->initializer = create_identifier_node(var);
    return n;
}

Node* make_statement_list(Node *first, Node *second) {
    if (!first) return second;
    Node *cur = first;
    while (cur->next) cur = cur->next;
    cur->next = second;
    return first;
}

void mark_nodes_in_loop(Node *body) {
    Node *cur = body;
    while (cur) { cur->is_in_loop = 1; cur = cur->next; }
}

/* ── Constructors used by tac.c ── */
ASTNode* create_constant_node(int value) {
    Node *n       = alloc_node(NODE_CONSTANT);
    n->init_value = value;
    return n;
}

ASTNode* create_identifier_node(char *name) {
    Node *n  = alloc_node(NODE_IDENTIFIER);
    n->name  = strdup(name);
    return n;
}

ASTNode* create_binary_op_node(char *op, ASTNode *left, ASTNode *right) {
    Node *n   = alloc_node(NODE_BINARY_OP);
    n->op_str = strdup(op);
    n->left   = left;
    n->right  = right;
    return n;
}

ASTNode* create_assignment_node(char *name, ASTNode *value) {
    Node *n       = alloc_node(NODE_ASSIGNMENT);
    n->var_name   = strdup(name);
    n->initializer= value;
    return n;
}

ASTNode* create_declaration_node(char *name, DataType dtype, ASTNode *init) {
    Node *n        = alloc_node(NODE_DECLARATION);
    n->var_name    = strdup(name);
    n->data_type   = dtype;
    n->initializer = init;
    return n;
}

/* ── print_ast ── */
void print_ast(Node *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");

    /* BUG 4 FIX: comparisons now use enum, not strcmp on strings */
    switch (node->type) {
        case NODE_FOR_LOOP:
            printf("FOR LOOP (cond: %s < %s, incr: %s)\n",
                   node->cond_var, node->cond_limit, node->incr_var);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("Init:\n");
            print_ast(node->init, indent+2);
            for (int i = 0; i < indent+1; i++) printf("  ");
            printf("Body:\n");
            print_ast(node->body, indent+2);
            break;

        case NODE_DECLARATION:
            printf("DECLARATION: %s",  node->var_name);
            if (node->has_init) printf(" = %d", node->init_value);
            if (node->is_in_loop) printf(" [IN LOOP]");
            printf("\n");
            break;

        case NODE_ASSIGNMENT:
            printf("ASSIGNMENT: %s = %d\n", node->var_name, node->init_value);
            break;

        case NODE_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->name ? node->name : "?");
            break;

        case NODE_CONSTANT:
            printf("CONSTANT: %d\n", node->init_value);
            break;

        default:
            printf("NODE(type=%d)\n", node->type);
            break;
    }

    print_ast(node->next, indent);
}

/* ── free_ast ── */
void free_ast(Node *node) {
    if (!node) return;
    free_ast(node->next);
    free_ast(node->init);
    free_ast(node->body);
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->initializer);
    free_ast(node->statements);
    free(node->var_name);
    free(node->cond_var);
    free(node->cond_limit);
    free(node->incr_var);
    free(node->op_str);
    free(node->name);
    free(node);
}
