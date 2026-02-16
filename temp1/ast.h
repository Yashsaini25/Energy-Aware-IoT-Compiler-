#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node types
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_COMPOUND,
    NODE_FOR_LOOP,
    NODE_WHILE_LOOP,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_IDENTIFIER,
    NODE_CONSTANT,
    NODE_IF_STMT,
    NODE_RETURN
} NodeType;

// Data types
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_VOID
} DataType;

// Base AST node
typedef struct ASTNode {
    NodeType type;
    int line_number;
    struct ASTNode *next;  // For linking statements
} ASTNode;

// Program node (root)
typedef struct {
    ASTNode base;
    ASTNode *functions;  // List of functions
} ProgramNode;

// Function node
typedef struct {
    ASTNode base;
    char *name;
    DataType return_type;
    ASTNode *parameters;
    ASTNode *body;
} FunctionNode;

// Compound statement (block of code)
typedef struct {
    ASTNode base;
    ASTNode *statements;  // Linked list of statements
    int statement_count;
} CompoundNode;

// For loop node
typedef struct {
    ASTNode base;
    ASTNode *init;        // Initialization (e.g., int i=0)
    ASTNode *condition;   // Condition (e.g., i<n)
    ASTNode *increment;   // Increment (e.g., i++)
    ASTNode *body;        // Loop body
    int estimated_iterations;  // For energy calculation
} ForLoopNode;

// While loop node
typedef struct {
    ASTNode base;
    ASTNode *condition;
    ASTNode *body;
    int estimated_iterations;
} WhileLoopNode;

// Variable declaration node
typedef struct {
    ASTNode base;
    char *var_name;
    DataType data_type;
    ASTNode *initializer;
    
    // LICM-specific fields
    int is_in_loop;              // Is this declaration inside a loop?
    int is_loop_invariant;       // Can it be hoisted?
    int defined_outside_loop;    // Is it defined before loop?
    struct ASTNode *dependent_on; // What does it depend on?
} DeclarationNode;

// Assignment node
typedef struct {
    ASTNode base;
    char *var_name;
    ASTNode *value;
    
    // LICM-specific fields
    int is_loop_invariant;       // Is RHS invariant?
    int modifies_loop_var;       // Does it change loop variable?
} AssignmentNode;

// Binary operation node
typedef struct {
    ASTNode base;
    char *operator;
    ASTNode *left;
    ASTNode *right;
    
    // LICM-specific
    int is_loop_invariant;       // Are both operands invariant?
} BinaryOpNode;

// Identifier node
typedef struct {
    ASTNode base;
    char *name;
} IdentifierNode;

// Constant node
typedef struct {
    ASTNode base;
    int int_value;
    float float_value;
    DataType value_type;
} ConstantNode;

// If statement node
typedef struct {
    ASTNode base;
    ASTNode *condition;
    ASTNode *then_branch;
    ASTNode *else_branch;
} IfNode;

// Return statement node
typedef struct {
    ASTNode base;
    ASTNode *return_value;
} ReturnNode;

// Function prototypes
ASTNode* create_program_node();
ASTNode* create_function_node(char *name, DataType return_type, ASTNode *body);
ASTNode* create_compound_node();
ASTNode* create_for_loop_node(ASTNode *init, ASTNode *cond, ASTNode *inc, ASTNode *body);
ASTNode* create_while_loop_node(ASTNode *condition, ASTNode *body);
ASTNode* create_declaration_node(char *name, DataType type, ASTNode *init);
ASTNode* create_assignment_node(char *name, ASTNode *value);
ASTNode* create_binary_op_node(char *op, ASTNode *left, ASTNode *right);
ASTNode* create_identifier_node(char *name);
ASTNode* create_constant_node(int value);
ASTNode* create_if_node(ASTNode *condition, ASTNode *then_br, ASTNode *else_br);
ASTNode* create_return_node(ASTNode *value);

void add_statement(CompoundNode *compound, ASTNode *statement);
void print_ast(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif