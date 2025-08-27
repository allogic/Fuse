%x MULTI_LINE_COMMENT
%x PRE_PROCESSOR_DIRECTIVE

%{
#include "enton/parser.h"

#define YY_NEW_FILE

#define YY_DO_BEFORE_ACTION           \
  (yytext_ptr) = yy_bp;               \
  yyleng = (int32_t) (yy_cp - yy_bp); \
  (yy_hold_char) = *yy_cp;            \
  *yy_cp = '\0';                      \
  (yy_c_buf_p) = yy_cp;

#define yywrap() 1
%}

%%
"struct"  { return STRUCT; }
"layout"  { return LAYOUT; }
"in"      { return IN; }
"uniform" { return UNIFORM; }

"," { return COMMA; }
":" { return COLON; }
";" { return SEMICOLON; }
"(" { return L_PAREN; }
")" { return R_PAREN; }
"{" { return L_BRACE; }
"}" { return R_BRACE; }
"=" { return EQ; }

"void"  { return VOID; }
"int"   { return INT; }
"vec3"  { return VEC3; }
"ivec3" { return IVEC3; }
"mat4"  { return MAT4; }

\n  {  }
\r  {  }
\t  {  }
" " {  }

"//"[^\r\n]*               {  }
"/*"                       { BEGIN(MULTI_LINE_COMMENT); }
<MULTI_LINE_COMMENT>.*"*/" { BEGIN(INITIAL); }

"#"                                    { BEGIN(PRE_PROCESSOR_DIRECTIVE); }
<PRE_PROCESSOR_DIRECTIVE>.*\\(\n|\r\n) {  }
<PRE_PROCESSOR_DIRECTIVE>.*(\n|\r\n)   { BEGIN(INITIAL); }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = core_string_from(yytext, yyleng); return IDENTIFIER; }
"\""[^"]*"\""          { yylval.string = core_string_from(yytext + 1, yyleng - 2); return STRING; }
[0-9]+                 { yylval.number = atoll(yytext + 1); return NUMBER; }
%%