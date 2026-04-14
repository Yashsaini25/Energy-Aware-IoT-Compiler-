#ifndef AST_H
#define AST_H

typedef enum{
    NODE_PROGRAM,
    NODE_FOR_LOOP,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_CONSTANT,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_IF_STMT,
    NODE_WHILE_LOOP,
    NODE_RETURN
}NodeType;

typedef enum{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_VOID
}DataType;

typedef struct Node{
    NodeType type;
    int line_number;

    /* declaration / assignment */
    char* var_name;
    int has_init;
    int init_value;
    int is_in_loop;
    DataType data_type;

    /* for loop fields */
    struct Node* init;
    char* cond_var;
    char* cond_limit;
    char* incr_var;
    struct Node* body;

    /* binary op */
    char* op_str;
    struct Node* left;
    struct Node* right;

    /* initializer expression(declaration) */
    struct Node* initializer;

    /* identifier / constant */
    char* name;
    int int_value;

    /* compound / statement list */
    struct Node* statements;
    int statement_count;

    struct Node* next;
}Node;

typedef Node ASTNode;
typedef Node BinaryOpNode;
typedef Node IdentifierNode;
typedef Node ConstantNode;
typedef Node AssignmentNode;
typedef Node DeclarationNode;
typedef Node ForLoopNode;
typedef Node ProgramNode;


Node* make_for_loop(Node* init, char* cond_var, char* limit, char* incr, Node* body);
Node* make_declaration(char* name);
Node* make_declaration_with_init(char* name, int value);
Node* make_assignment(char* name, int value);
Node* make_assignment_var(char* name, char* var);
Node* make_statement_list(Node* first, Node* second);

void print_ast(Node* node, int indent);
void mark_nodes_in_loop(Node* body);
void free_ast(Node* node);

ASTNode* create_constant_node(int value);
ASTNode* create_identifier_node(char* name);
ASTNode* create_binary_op_node(char* op, ASTNode* left, ASTNode* right);
ASTNode* create_assignment_node(char* name, ASTNode* value);
ASTNode* create_declaration_node(char* name, DataType dtype, ASTNode* init);

#endif