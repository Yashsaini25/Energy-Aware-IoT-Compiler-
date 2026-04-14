/* ast.h - AST for our compiler */
#ifndef AST_H
#define AST_H

/*
 * BUG 1: Two separate type systems in same header.
 *   Simple `Node` (used by parser/ast.c) + ASTNode subtypes (used by tac.c).
 *   tac.c was casting Node* to ASTNode* with different layouts → UB.
 * FIX: ONE unified Node struct. ASTNode is just a typedef alias.
 *
 * BUG 2: Node used `char *type` string; tac.c compared NodeType enum values.
 *   They could never agree on node kind.
 * FIX: Replace char*type with NodeType enum in Node struct.
 * 
 * BUG 3: tac.c cast Node* to BinaryOpNode*, IdentifierNode* etc.
 *   Those were separate structs with different field layouts → garbage reads.
 * FIX: All "subtype" typedefs now alias the same Node struct.
 */

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

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_VOID
} DataType;

typedef struct Node {
    NodeType type;       /* was char *type string — now enum        */
    int line_number;

    /* declaration / assignment */
    char    *var_name;
    int      has_init;
    int      init_value;
    DataType data_type;
    int      is_in_loop;

    /* for-loop fields */
    struct Node *init;
    char        *cond_var;
    char        *cond_limit;
    char        *incr_var;
    struct Node *body;

    /* binary op */
    char        *op_str;     /* "+", "-", "*", "/", "<", … */
    struct Node *left;
    struct Node *right;

    /* initializer expression (declaration) */
    struct Node *initializer;

    /* identifier / constant */
    char  *name;
    float  float_value;

    /* compound / statement list */
    struct Node *statements;
    int          statement_count;

    /* linked list of siblings */
    struct Node *next;
} Node;

/* tac.c uses ASTNode — make it an alias for Node */
typedef Node ASTNode;

/* All subtype casts in tac.c now resolve to the same struct */
typedef Node BinaryOpNode;
typedef Node IdentifierNode;
typedef Node ConstantNode;
typedef Node AssignmentNode;
typedef Node DeclarationNode;
typedef Node ForLoopNode;
typedef Node CompoundNode;
typedef Node FunctionNode;
typedef Node ProgramNode;

/* Constructor functions (ast.c) */
Node* make_for_loop(Node *init, char *cond_var, char *limit,
                    char *incr, Node *body);
Node* make_declaration(char *name);
Node* make_declaration_with_init(char *name, int value);
Node* make_assignment(char *name, int value);
Node* make_assignment_var(char *name, char *var);
Node* make_statement_list(Node *first, Node *second);

void print_ast(Node *node, int indent);
void mark_nodes_in_loop(Node *body);
void free_ast(Node *node);

/* Constructors also used by tac.c */
ASTNode* create_constant_node(int value);
ASTNode* create_identifier_node(char *name);
ASTNode* create_binary_op_node(char *op, ASTNode *left, ASTNode *right);
ASTNode* create_assignment_node(char *name, ASTNode *value);
ASTNode* create_declaration_node(char *name, DataType dtype, ASTNode *init);

#endif /* AST_H */
