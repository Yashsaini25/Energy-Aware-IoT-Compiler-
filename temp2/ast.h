/* ast.h - AST for our compiler */
#ifndef AST_H
#define AST_H

typedef struct Node {
    char *type;  // "for_loop", "declaration", "assignment"
    
    // For declarations
    char *var_name;
    int has_init;
    int init_value;
    
    // For loops
    struct Node *init;
    char *cond_var;
    char *cond_limit;
    char *incr_var;
    struct Node *body;
    
    // For statement lists
    struct Node *next;
    
    // For optimization
    int is_in_loop;  // Is this inside a loop?
} Node;

/* Constructor functions */
Node* make_for_loop(Node *init, char *cond_var, char *limit, 
                    char *incr, Node *body);
Node* make_declaration(char *name);
Node* make_declaration_with_init(char *name, int value);
Node* make_assignment(char *name, int value);
Node* make_assignment_var(char *name, char *var);
Node* make_statement_list(Node *first, Node *second);

/* Utility functions */
void print_ast(Node *node, int indent);
void mark_nodes_in_loop(Node *body);

#endif