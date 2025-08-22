%x MULTI_LINE_COMMENT

%{
#include "enton/parser.h"

#ifndef UPDATE_CURSOR_LOCATION_SINGLE_LINE
	#define UPDATE_CURSOR_LOCATION_SINGLE_LINE() \
		yylloc.first_line = g_line_number; \
		yylloc.first_column = g_column_number; \
		g_column_number += yyleng; \
		yylloc.last_line = g_line_number; \
		yylloc.last_column = g_column_number - 1
#endif // UPDATE_CURSOR_LOCATION_SINGLE_LINE

#ifndef UPDATE_CURSOR_LOCATION_MULTI_LINE
	#define UPDATE_CURSOR_LOCATION_MULTI_LINE() \
		yylloc.first_line = g_line_number; \
		yylloc.first_column = g_column_number; \
		{ \
			char const* ptr = yytext; \
			while (*ptr) \
			{ \
				if (*ptr == '\r') \
				{ \
					g_column_number = 1; \
				} \
				else if (*ptr == '\n') \
				{ \
					g_line_number++; \
				} \
				else \
				{ \
					g_column_number++; \
				} \
				ptr++; \
			} \
		} \
		yylloc.last_line = g_line_number; \
		yylloc.last_column = g_column_number
#endif // UPDATE_CURSOR_LOCATION_MULTI_LINE

#ifndef UPDATE_CURSOR_LOCATION_CARRIAGE_RETURN
	#define UPDATE_CURSOR_LOCATION_CARRIAGE_RETURN() \
		yylloc.first_line = g_line_number; \
		yylloc.first_column = g_column_number; \
		g_column_number = 1; \
		yylloc.last_line = g_line_number; \
		yylloc.last_column = g_column_number
#endif // UPDATE_CURSOR_LOCATION_CARRIAGE_RETURN

#ifndef UPDATE_CURSOR_LOCATION_NEW_LINE
	#define UPDATE_CURSOR_LOCATION_NEW_LINE() \
		yylloc.first_line = g_line_number; \
		yylloc.first_column = g_column_number; \
		g_line_number++; \
		yylloc.last_line = g_line_number; \
		yylloc.last_column = g_column_number
#endif // UPDATE_CURSOR_LOCATION_NEW_LINE
%}

%%
"version" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return VERSION;
}

"core" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return CORE;
}

"extension" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return EXTENSION;
}

"struct" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return STRUCT;
}

"require" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return QUALIFIER;
}

"layout" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return LAYOUT;
}

"location" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return LOCATION;
}

"binding" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return BINDING;
}

"in" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return IN;
}

"uniform" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return UNIFORM;
}

"," {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return COMMA;
}

"#" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return HASH;
}

":" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return COLON;
}

";" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return SEMICOLON;
}

"(" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return LPAREN;
}

")" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return RPAREN;
}

"{" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return LBRACE;
}

"}" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return RBRACE;
}

"=" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return EQ;
}

"void" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return VOID;
}

"int" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return INT;
}

"vec3" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return VEC3;
}

"ivec3" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return IVEC3;
}

"mat4" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return MAT4;
}

"\r" {
	UPDATE_CURSOR_LOCATION_CARRIAGE_RETURN();
}

"\n" {
	UPDATE_CURSOR_LOCATION_NEW_LINE();
}

"\t" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
}

" " {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
}

"//"[^\r\n]* {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
}

"/*" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	BEGIN(MULTI_LINE_COMMENT);
}

<MULTI_LINE_COMMENT>"\r" {
	UPDATE_CURSOR_LOCATION_CARRIAGE_RETURN();
}

<MULTI_LINE_COMMENT>"\n" {
	UPDATE_CURSOR_LOCATION_NEW_LINE();
}

<MULTI_LINE_COMMENT>. {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
}

<MULTI_LINE_COMMENT>"*/" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	BEGIN(INITIAL);
}

[a-zA-Z_][a-zA-Z0-9_]* {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	yylval.string = core_string_from(yytext, yyleng);
	return IDENT;
}

"\""[^"]*"\"" {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	yylval.string = core_string_from(yytext + 1, yyleng - 2);
	return STRING;
}

[0-9]+ {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	yylval.number = atoll(yytext + 1);
	return NUMBER;
}

. {
	UPDATE_CURSOR_LOCATION_SINGLE_LINE();
	return OTHER;
}
%%
