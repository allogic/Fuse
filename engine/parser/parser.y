%define parse.assert
%define parse.error verbose

%start PROGRAM

%locations

%code requires
{
#ifdef _WIN32
	#include <io.h>

	#define fileno _fileno
	#define isatty _isatty
#endif // _WIN32

	// flex --header-file="./lexer.h" --outfile="./lexer.c" --yylineno --nounistd "./lexer.l"
	// bison --header="./parser.h" --output="./parser.c" --locations "./parser.y"

	#include <stdio.h>
	#include <stdint.h>
	#include <stdarg.h>

	#include "engine/core/api.h"

	#include "engine/parser/config.h"
	#include "engine/parser/context.h"
	#include "engine/parser/expression.h"
	//#include "engine/parser/lexer.h"

	extern char const* g_current_filename;
	extern char* yytext;
	extern int32_t g_line_number;
	extern int32_t g_column_number;
	extern int32_t yyleng;

	extern int yylex(void);
	extern int32_t yyerror(char const* msg, ...);
	extern int32_t yywrap(void);

	void parser_parse_file(char const* file_path);
}

%union
{
	core_string_t string;
	parser_expression_t expression;
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

char const* g_current_filename;
int32_t g_line_number;
int32_t g_column_number;

int32_t yyerror(char const* fmt, ...) {
	static char fmt_buffer[PARSER_LOG_FMT_BUFFER_SIZE];

	va_list args;
	va_start(args, fmt);
	vsnprintf(fmt_buffer, PARSER_LOG_FMT_BUFFER_SIZE, fmt, args);
	printf("%s:%d:%d: %s\n", g_current_filename, yylloc.first_line, yylloc.first_column, fmt_buffer);
	va_end(args);

	return 1;
}
int32_t yywrap(void) {
	return 1;
}
void parser_parse_file(char const* file_path) {
	g_current_filename = file_path;
	g_line_number = 1;
	g_column_number = 1;

	uint8_t *file_buffer = 0;
	uint64_t file_length = 0;

	if (core_filesystem_read_text(&file_buffer, &file_length, file_path))
	{
		//ctx_alloc();
		//ctx_push_scope();

		//YY_BUFFER_STATE buf1 = yy_scan_string(expr1);
		yy_switch_to_buffer(file_buffer);
		yyparse();
		//yy_delete_buffer(buf1);

		//ctx_print();
		//ctx_pop_scope();
		//ctx_free();

		core_heap_free(file_buffer);
	}

	printf("DONE\n");
}
