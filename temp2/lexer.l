/* lexer.l - Simplified for loops only */
%{
#include "parser.tab.h"
#include <string.h>
%}

%%

"for"       { return FOR; }
"int"       { return INT; }

[a-zA-Z][a-zA-Z0-9]*  { 
    yylval.string = strdup(yytext); 
    return ID; 
}

[0-9]+      { 
    yylval.num = atoi(yytext); 
    return NUMBER; 
}

"<"         { return LT; }
"="         { return ASSIGN; }
"++"        { return INCR; }

"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
";"         { return SEMICOLON; }

[ \t\n]+    { /* skip whitespace */ }

.           { printf("Unknown: %s\n", yytext); }

%%

int yywrap() { return 1; }