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
	primitive_type_t primitive_type;
}

%token VERSION
%token CORE
%token EXTENSION
%token STRUCT
%token QUALIFIER
%token LAYOUT
%token IN
%token UNIFORM

%token HASH
%token COMMA
%token COLON
%token SEMICOLON
%token L_PAREN R_PAREN
%token L_BRACE R_BRACE
%token EQ

%token VOID
%token INT
%token VEC3
%token IVEC3
%token MAT4

%token IDENTIFIER
%token STRING
%token NUMBER

%type <string> IDENTIFIER
%type <string> STRING

%type <number> NUMBER

%type <expression> PREPROC_DECL
%type <expression> VERSION_DECL
%type <expression> EXTENSION_DECL

%type <expression> MODIFIER_DECL
%type <expression> MODIFIER

%type <expression> LAYOUT_DECL
%type <expression> LAYOUT_INPUT_DECL
%type <expression> LAYOUT_UNIFORM_DECL

%type <expression> STRUCT_DECL
%type <expression> STRUCT_TYPE_DECL
%type <expression> STRUCT_MEMBER

%type <primitive_type> PRIMITIVE_TYPE

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
	: HASH EXTENSION IDENTIFIER COLON QUALIFIER { $$ = expression_extension(expression_identifier($3)); }
	;

///////////////////////////////////////////////////////////////
// Layout Declaration
///////////////////////////////////////////////////////////////

LAYOUT_DECL
	: LAYOUT_INPUT_DECL
	| LAYOUT_UNIFORM_DECL
	;

LAYOUT_INPUT_DECL
	: LAYOUT MODIFIER_DECL IN PRIMITIVE_TYPE IDENTIFIER SEMICOLON
		{
			$$ = expression_layout_input(
				$2,
				expression_primitive($4),
				expression_identifier($5));
		}
	;

LAYOUT_UNIFORM_DECL
	: LAYOUT MODIFIER_DECL UNIFORM STRUCT_DECL IDENTIFIER SEMICOLON
		{
			$$ = expression_layout_uniform(
				$2,
				$4,
				expression_identifier($5));
		}
	;

///////////////////////////////////////////////////////////////
// Modifier Declaration
///////////////////////////////////////////////////////////////

MODIFIER_DECL
	: { context_push_expression_vector(); } L_PAREN MODIFIER_VECTOR R_PAREN { $$ = expression_modifier(expression_packv(context_pop_expression_vector())); }
	;

MODIFIER_VECTOR
	: MODIFIER COMMA MODIFIER_VECTOR { context_push_expression($1); }
	| MODIFIER { context_push_expression($1); }
	| %empty
	;

MODIFIER
	: IDENTIFIER EQ NUMBER { $$ = expression_assignment(expression_identifier($1), expression_number($3)); }
	;

///////////////////////////////////////////////////////////////
// Struct Declaration
///////////////////////////////////////////////////////////////

STRUCT_DECL
	: { context_push_expression_vector(); } STRUCT_TYPE_DECL L_BRACE STRUCT_MEMBER_VECTOR R_BRACE { $$ = expression_struct($2, expression_packv(context_pop_expression_vector())); }
	;

STRUCT_TYPE_DECL
	: IDENTIFIER { $$ = expression_type($1); }
	;

STRUCT_MEMBER_VECTOR
	: STRUCT_MEMBER SEMICOLON STRUCT_MEMBER_VECTOR { context_push_expression($1); }
	| STRUCT_MEMBER { context_push_expression($1); }
	| %empty
	;

STRUCT_MEMBER
	: PRIMITIVE_TYPE IDENTIFIER { $$ = expression_struct_member(expression_primitive($1), expression_identifier($2)); }
	;

///////////////////////////////////////////////////////////////
// Function Declaration
///////////////////////////////////////////////////////////////

FUNCTION_DECL
	: PRIMITIVE_TYPE IDENTIFIER FUNCTION_ARG_PACK
	;

FUNCTION_ARG_PACK
	: L_PAREN FUNCTION_ARG R_PAREN

FUNCTION_ARG
	: PRIMITIVE_TYPE COMMA FUNCTION_ARG
	| PRIMITIVE_TYPE
	| %empty
	;

///////////////////////////////////////////////////////////////
// Primitive Types
///////////////////////////////////////////////////////////////

PRIMITIVE_TYPE
	: VOID { $$ = PRIMITIVE_TYPE_VOID; }
	| INT { $$ = PRIMITIVE_TYPE_INT; }
	| VEC3 { $$ = PRIMITIVE_TYPE_VEC3; }
	| IVEC3 { $$ = PRIMITIVE_TYPE_IVEC3; }
	| MAT4 { $$ = PRIMITIVE_TYPE_MAT4; }
	;
%%

#include "enton/epilogue.inl"
