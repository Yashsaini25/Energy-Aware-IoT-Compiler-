/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     NUMBER = 259,
     FLOAT_NUM = 260,
     INT = 261,
     FLOAT = 262,
     CHAR = 263,
     VOID = 264,
     FOR = 265,
     WHILE = 266,
     IF = 267,
     ELSE = 268,
     RETURN = 269,
     LPAREN = 270,
     RPAREN = 271,
     LBRACE = 272,
     RBRACE = 273,
     SEMICOLON = 274,
     COMMA = 275,
     ASSIGN = 276,
     INCREMENT = 277,
     DECREMENT = 278,
     LT = 279,
     GT = 280,
     LE = 281,
     GE = 282,
     EQ = 283,
     NE = 284,
     PLUS = 285,
     MINUS = 286,
     MULT = 287,
     DIV = 288,
     MOD = 289,
     LSHIFT = 290,
     RSHIFT = 291,
     AND = 292,
     OR = 293
   };
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define NUMBER 259
#define FLOAT_NUM 260
#define INT 261
#define FLOAT 262
#define CHAR 263
#define VOID 264
#define FOR 265
#define WHILE 266
#define IF 267
#define ELSE 268
#define RETURN 269
#define LPAREN 270
#define RPAREN 271
#define LBRACE 272
#define RBRACE 273
#define SEMICOLON 274
#define COMMA 275
#define ASSIGN 276
#define INCREMENT 277
#define DECREMENT 278
#define LT 279
#define GT 280
#define LE 281
#define GE 282
#define EQ 283
#define NE 284
#define PLUS 285
#define MINUS 286
#define MULT 287
#define DIV 288
#define MOD 289
#define LSHIFT 290
#define RSHIFT 291
#define AND 292
#define OR 293




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 13 "parser.y"
{
    int int_val;
    float float_val;
    char *string;
    void *ptr;
}
/* Line 1529 of yacc.c.  */
#line 132 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

