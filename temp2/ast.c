/* ast.c - Simple AST implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* make_for_loop(Node *init, char *cond_var, char *limit, 
                    char *incr, Node *body) {
    Node *n = malloc(sizeof(Node));
    n->type = "for_loop";
    n->init = init;
    n->cond_var = strdup(cond_var);
    n->cond_limit = strdup(limit);
    n->incr_var = strdup(incr);
    n->body = body;
    n->next = NULL;
    
    // Mark all statements in body as being in loop
    mark_nodes_in_loop(body);
    
    return n;
}

Node* make_declaration(char *name) {
    Node *n = malloc(sizeof(Node));
    n->type = "declaration";
    n->var_name = strdup(name);
    n->has_init = 0;
    n->next = NULL;
    n->is_in_loop = 0;
    return n;
}

Node* make_declaration_with_init(char *name, int value) {
    Node *n = malloc(sizeof(Node));
    n->type = "declaration";
    n->var_name = strdup(name);
    n->has_init = 1;
    n->init_value = value;
    n->next = NULL;
    n->is_in_loop = 0;
    return n;
}

Node* make_assignment(char *name, int value) {
    Node *n = malloc(sizeof(Node));
    n->type = "assignment";
    n->var_name = strdup(name);
    n->init_value = value;
    n->next = NULL;
    n->is_in_loop = 0;
    return n;
}

Node* make_assignment_var(char *name, char *var) {
    Node *n = malloc(sizeof(Node));
    n->type = "assignment";
    n->var_name = strdup(name);
    n->next = NULL;
    n->is_in_loop = 0;
    return n;
}

Node* make_statement_list(Node *first, Node *second) {
    if (!first) return second;
    
    // Find end of first list
    Node *current = first;
    while (current->next) {
        current = current->next;
    }
    current->next = second;
    
    return first;
}

void mark_nodes_in_loop(Node *body) {
    Node *current = body;
    while (current) {
        current->is_in_loop = 1;
        current = current->next;
    }
}

void print_ast(Node *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    if (strcmp(node->type, "for_loop") == 0) {
        printf("FOR LOOP\n");
        for (int i = 0; i < indent+1; i++) printf("  ");
        printf("Init:\n");
        print_ast(node->init, indent+2);
        for (int i = 0; i < indent+1; i++) printf("  ");
        printf("Body:\n");
        print_ast(node->body, indent+2);
    }
    else if (strcmp(node->type, "declaration") == 0) {
        printf("DECLARATION: %s", node->var_name);
        if (node->is_in_loop) printf(" [IN LOOP]");
        printf("\n");
    }
    else if (strcmp(node->type, "assignment") == 0) {
        printf("ASSIGNMENT: %s = %d\n", node->var_name, node->init_value);
    }
    
    print_ast(node->next, indent);
}