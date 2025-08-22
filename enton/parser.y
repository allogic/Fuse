%define parse.assert
%define parse.error verbose

%start PROGRAM

%locations

%code requires
{
#include "enton/prologue.inl"
}

%union
{
	core_string_t string;
	enton_expression_t expression;
}

%token IDENT STRING
%token STRUCT
%token SEMICOLON
%token LPAREN RPAREN
%token LBRACE RBRACE

%type <string> IDENT STRING
%type <expression> STRUCT

%%
PROGRAM
	: PROGRAM STRUCT_DECL {  }
	| %empty
	;

STRUCT_DECL
	: STRUCT IDENT LPAREN RPAREN SEMICOLON
	;
%%

#include "enton/epilogue.inl"
