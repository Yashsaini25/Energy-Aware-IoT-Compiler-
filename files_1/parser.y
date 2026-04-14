/* parser.y - Parse simple for loops */
%{
#include <stdio.h>
#include "ast.h"

int yylex();
void yyerror(char *s);

Node *root = NULL;  /* Root of AST */
%}

%union {
    int num;
    char *string;
    struct Node *node;
}

%token <string> ID
%token <num> NUMBER
%token FOR INT
%token LT ASSIGN INCR
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON

%type <node> program for_loop statement_list statement declaration assignment

%%

program:
    for_loop { 
        root = $1; 
        printf("Parsed successfully!\n");
    }
    ;

for_loop:
    FOR LPAREN declaration SEMICOLON 
            ID LT ID SEMICOLON 
            ID INCR RPAREN 
        LBRACE statement_list RBRACE
    {
        $$ = make_for_loop($3, $5, $7, $9, $13);
    }
    ;

statement_list:
    statement {
        $$ = $1;
    }
    | statement_list statement {
        $$ = make_statement_list($1, $2);
    }
    ;

statement:
    declaration SEMICOLON { $$ = $1; }
    | assignment SEMICOLON { $$ = $1; }
    ;

declaration:
    INT ID {
        $$ = make_declaration($2);
    }
    | INT ID ASSIGN NUMBER {
        $$ = make_declaration_with_init($2, $4);
    }
    ;

assignment:
    ID ASSIGN NUMBER {
        $$ = make_assignment($1, $3);
    }
    | ID ASSIGN ID {
        $$ = make_assignment_var($1, $3);
    }
    ;

%%

void yyerror(char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}