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
	core_vector_t vector;
	int64_t number;
	expression_t expression;
	datatype_t datatype;
}

%token VERSION
%token CORE
%token EXTENSION
%token STRUCT
%token QUALIFIER
%token LAYOUT
%token LOCATION
%token BINDING
%token IN
%token UNIFORM

%token HASH
%token COMMA
%token COLON
%token SEMICOLON
%token LPAREN RPAREN
%token LBRACE RBRACE
%token EQ

%token VOID
%token INT
%token VEC3
%token IVEC3
%token MAT4

%token IDENTIFIER STRING
%token NUMBER

%type <string> IDENTIFIER STRING
%type <number> NUMBER
%type <expression> PREPROC_DECL VERSION_DECL EXTENSION_DECL STRUCT_DECL LAYOUT_DECL ASSIGNMENT MODIFIER_PACK
%type <datatype> PRIMITIVE_TYPE

%%
PROGRAM
	: PROGRAM PREPROC_DECL { context_push_preproc_decl($2); }
	| PROGRAM STRUCT_DECL { context_push_struct_decl($2); }
	| PROGRAM LAYOUT_DECL { context_push_layout_decl($2); }
	| PROGRAM FUNCTION_DECL { YYACCEPT; }
	| %empty
	;

PREPROC_DECL
	: VERSION_DECL
	| EXTENSION_DECL
	;

VERSION_DECL
	: HASH VERSION NUMBER { $$ = expression_version($3); }
	| HASH VERSION NUMBER CORE { $$ = expression_version($3); }
	;

EXTENSION_DECL
	: HASH EXTENSION IDENTIFIER COLON QUALIFIER { $$ = expression_extension($3); }
	;

LAYOUT_DECL
	: LAYOUT LPAREN MODIFIER_PACK RPAREN IN PRIMITIVE_TYPE IDENTIFIER SEMICOLON
		{
			$$ = expression_layout_input(
				$3,
				$6,
				$7);
		}
	| LAYOUT LPAREN MODIFIER_PACK RPAREN UNIFORM STRUCT_DECL IDENTIFIER SEMICOLON
		{
			$$ = expression_layout_uniform(
				$3,
				$6,
				$7);
		}
	;

MODIFIER_PACK
	: { context_push_expression_vector(); } LPAREN MODIFIER_LIST RPAREN { $$ = expression_packv(context_pop_expression_vector()); }
	;

MODIFIER_LIST
	: ASSIGNMENT COMMA MODIFIER_LIST { context_push_expression(expression_modifier($1)); }
	| ASSIGNMENT { context_push_expression(expression_modifier($1)); }
	| %empty
	;

ASSIGNMENT
	: IDENTIFIER EQ NUMBER { $$ = expression_assign($1, $3); }
	;

///////////////////////////////////////////////////////////////
// Struct Declaration
///////////////////////////////////////////////////////////////

STRUCT_DECL
	: { context_push_expression_vector(); } PRIMITIVE_TYPE LPAREN STRUCT_MEMBER_LIST RPAREN { $$ = expression_struct($1, context_pop_expression_vector()); }
	;

STRUCT_MEMBER_LIST
	: STRUCT_MEMBER COMMA MODIFIER_LIST { context_push_expression(expression_modifier($1)); }
	| STRUCT_MEMBER { context_push_expression(expression_modifier($1)); }
	| %empty
	;

STRUCT_MEMBER
	: PRIMITIVE_TYPE IDENTIFIER SEMICOLON { $$ = expression_struct_member($1, $3); }
	;

///////////////////////////////////////////////////////////////
// Function Declaration
///////////////////////////////////////////////////////////////

FUNCTION_DECL
	: PRIMITIVE_TYPE IDENTIFIER FUNCTION_ARG_PACK
	;

FUNCTION_ARG_PACK
	: LPAREN FUNCTION_ARG RPAREN

FUNCTION_ARG
	: PRIMITIVE_TYPE COMMA FUNCTION_ARG
	| PRIMITIVE_TYPE
	| %empty
	;

PRIMITIVE_TYPE
	: VOID { $$ = DATATYPE_VOID; }
	| INT { $$ = DATATYPE_INT; }
	| VEC3 { $$ = DATATYPE_VEC3; }
	| IVEC3 { $$ = DATATYPE_IVEC3; }
	| MAT4 { $$ = DATATYPE_MAT4; }
	;
%%

#include "enton/epilogue.inl"
