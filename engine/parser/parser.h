/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_H_INCLUDED
#define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
#  define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 9 "./parser.y"

#ifdef _WIN32
#  define fileno _fileno
#  define isatty _isatty
#endif // _WIN32

#include <stdint.h>
#include <stdarg.h>

#include "engine/core/api.h"

#include "engine/parser/config.h"
#include "engine/parser/context.h"
#include "engine/parser/expression.h"

extern char const *g_current_filename;
extern char *yytext;
extern int32_t g_line_number;
extern int32_t g_column_number;
extern int32_t yyleng;

extern int32_t yyerror(char const *msg, ...);
extern int32_t yywrap(void);

void parser_parse_file(char const *file_path);

#line 79 "./parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
#  define YYTOKENTYPE
enum yytokentype {
  YYEMPTY = -2,
  YYEOF = 0,       /* "end of file"  */
  YYerror = 256,   /* error  */
  YYUNDEF = 257,   /* "invalid token"  */
  IDENT = 258,     /* IDENT  */
  STRING = 259,    /* STRING  */
  STRUCT = 260,    /* STRUCT  */
  SEMICOLON = 261, /* SEMICOLON  */
  LPAREN = 262,    /* LPAREN  */
  RPAREN = 263,    /* RPAREN  */
  LBRACE = 264,    /* LBRACE  */
  RBRACE = 265     /* RBRACE  */
};
typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
union YYSTYPE {
#  line 40 "./parser.y"

  core_string_t string;
  parser_expression_t expression;

#  line 111 "./parser.h"
};
typedef union YYSTYPE YYSTYPE;
#  define YYSTYPE_IS_TRIVIAL 1
#  define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
#  define YYLTYPE_IS_DECLARED 1
#  define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse(void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
