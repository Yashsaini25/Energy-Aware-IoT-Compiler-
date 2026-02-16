%{
#include "parser.tab.h"
#include <string.h>
%}

%option yylineno

%%

"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
"void"          { return VOID; }
"for"           { return FOR; }
"while"         { return WHILE; }
"if"            { return IF; }
"else"          { return ELSE; }
"return"        { return RETURN; }

[a-zA-Z_][a-zA-Z0-9_]*  { 
    yylval.string = strdup(yytext); 
    return IDENTIFIER; 
}

[0-9]+          { 
    yylval.int_val = atoi(yytext); 
    return NUMBER; 
}

[0-9]+\.[0-9]+  { 
    yylval.float_val = atof(yytext); 
    return FLOAT_NUM; 
}

"<"             { yylval.string = strdup(yytext); return LT; }
">"             { yylval.string = strdup(yytext); return GT; }
"<="            { yylval.string = strdup(yytext); return LE; }
">="            { yylval.string = strdup(yytext); return GE; }
"=="            { yylval.string = strdup(yytext); return EQ; }
"!="            { yylval.string = strdup(yytext); return NE; }

"+"             { yylval.string = strdup(yytext); return PLUS; }
"-"             { yylval.string = strdup(yytext); return MINUS; }
"*"             { yylval.string = strdup(yytext); return MULT; }
"/"             { yylval.string = strdup(yytext); return DIV; }
"%"             { yylval.string = strdup(yytext); return MOD; }

"="             { return ASSIGN; }
"++"            { return INCREMENT; }
"--"            { return DECREMENT; }

"<<"            { yylval.string = strdup(yytext); return LSHIFT; }
">>"            { yylval.string = strdup(yytext); return RSHIFT; }
"&"             { yylval.string = strdup(yytext); return AND; }
"|"             { yylval.string = strdup(yytext); return OR; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMICOLON; }
","             { return COMMA; }

[ \t\r]+        { /* skip */ }
\n              { /* skip */ }

"//".*          { /* skip single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* skip multi-line comments */ }

.               { 
    fprintf(stderr, "Unknown character: %s at line %d\n", 
            yytext, yylineno); 
}

%%

int yywrap() {
    return 1;
}