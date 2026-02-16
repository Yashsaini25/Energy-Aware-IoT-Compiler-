%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
void yyerror(const char *s);

ASTNode *root = NULL;
%}

%union {
    int int_val;
    float float_val;
    char *string;
    void *ptr;
}

/* Token declarations */
%token <string> IDENTIFIER
%token <int_val> NUMBER
%token <float_val> FLOAT_NUM
%token INT FLOAT CHAR VOID
%token FOR WHILE IF ELSE RETURN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token ASSIGN INCREMENT DECREMENT
%token <string> LT GT LE GE EQ NE
%token <string> PLUS MINUS MULT DIV MOD
%token <string> LSHIFT RSHIFT AND OR

/* Type declarations */
%type <ptr> program function_list function compound_statement
%type <ptr> statement_list statement for_loop while_loop
%type <ptr> declaration assignment expression
%type <ptr> init_statement condition_expression increment_expression
%type <int_val> type_specifier

/* Operator precedence */
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left LSHIFT RSHIFT
%left PLUS MINUS
%left MULT DIV MOD

%%

program:
    function_list {
        root = create_program_node();
        ((ProgramNode*)root)->functions = $1;
        $$ = root;
    }
    ;

function_list:
    function {
        $$ = $1;
    }
    | function_list function {
        ASTNode *current = $1;
        while (current->next) current = current->next;
        current->next = $2;
        $$ = $1;
    }
    ;

function:
    type_specifier IDENTIFIER LPAREN RPAREN compound_statement {
        $$ = create_function_node($2, $1, $5);
        free($2);
    }
    ;

type_specifier:
    INT { $$ = TYPE_INT; }
    | FLOAT { $$ = TYPE_FLOAT; }
    | CHAR { $$ = TYPE_CHAR; }
    | VOID { $$ = TYPE_VOID; }
    ;

compound_statement:
    LBRACE statement_list RBRACE {
        $$ = $2;
    }
    | LBRACE RBRACE {
        $$ = create_compound_node();
    }
    ;

statement_list:
    statement {
        CompoundNode *compound = (CompoundNode*)create_compound_node();
        add_statement(compound, $1);
        $$ = (ASTNode*)compound;
    }
    | statement_list statement {
        add_statement((CompoundNode*)$1, $2);
        $$ = $1;
    }
    ;

statement:
    declaration SEMICOLON { $$ = $1; }
    | assignment SEMICOLON { $$ = $1; }
    | for_loop { $$ = $1; }
    | while_loop { $$ = $1; }
    | compound_statement { $$ = $1; }
    | RETURN expression SEMICOLON { $$ = create_return_node($2); }
    | RETURN SEMICOLON { $$ = create_return_node(NULL); }
    ;

for_loop:
    FOR LPAREN init_statement SEMICOLON 
             condition_expression SEMICOLON 
             increment_expression RPAREN 
        statement {
        // Wrap single statement in compound if needed
        ASTNode *body = $9;
        if (body->type != NODE_COMPOUND) {
            CompoundNode *compound = (CompoundNode*)create_compound_node();
            add_statement(compound, body);
            body = (ASTNode*)compound;
        }
        $$ = create_for_loop_node($3, $5, $7, body);
    }
    ;

while_loop:
    WHILE LPAREN expression RPAREN compound_statement {
        $$ = create_while_loop_node($3, $5);
    }
    ;

init_statement:
    declaration { $$ = $1; }
    | assignment { $$ = $1; }
    ;

condition_expression:
    expression { $$ = $1; }
    ;

increment_expression:
    assignment { $$ = $1; }
    | IDENTIFIER INCREMENT {
        ASTNode *one = create_constant_node(1);
        ASTNode *var = create_identifier_node($1);
        ASTNode *add = create_binary_op_node("+", var, one);
        $$ = create_assignment_node($1, add);
        free($1);
    }
    | IDENTIFIER DECREMENT {
        ASTNode *one = create_constant_node(1);
        ASTNode *var = create_identifier_node($1);
        ASTNode *sub = create_binary_op_node("-", var, one);
        $$ = create_assignment_node($1, sub);
        free($1);
    }
    ;

declaration:
    type_specifier IDENTIFIER {
        $$ = create_declaration_node($2, $1, NULL);
        free($2);
    }
    | type_specifier IDENTIFIER ASSIGN expression {
        $$ = create_declaration_node($2, $1, $4);
        free($2);
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression {
        $$ = create_assignment_node($1, $3);
        free($1);
    }
    ;

expression:
    IDENTIFIER {
        $$ = create_identifier_node($1);
        free($1);
    }
    | NUMBER {
        $$ = create_constant_node($1);
    }
    | expression PLUS expression {
        $$ = create_binary_op_node("+", $1, $3);
    }
    | expression MINUS expression {
        $$ = create_binary_op_node("-", $1, $3);
    }
    | expression MULT expression {
        $$ = create_binary_op_node("*", $1, $3);
    }
    | expression DIV expression {
        $$ = create_binary_op_node("/", $1, $3);
    }
    | expression MOD expression {
        $$ = create_binary_op_node("%", $1, $3);
    }
    | expression LT expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression GT expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression LE expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression GE expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression EQ expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression NE expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression LSHIFT expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | expression RSHIFT expression {
        $$ = create_binary_op_node($2, $1, $3);
        free($2);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", yylineno, s);
}