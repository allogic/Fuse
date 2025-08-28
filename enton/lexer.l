%x ACTIVE_MODE
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
"#define ENTON_BEGIN" { BEGIN(ACTIVE_MODE); }

\n  {  }
\r  {  }
\t  {  }
" " {  }

. {  }

<ACTIVE_MODE>"struct"  { return STRUCT; }
<ACTIVE_MODE>"layout"  { return LAYOUT; }
<ACTIVE_MODE>"in"      { return IN; }
<ACTIVE_MODE>"uniform" { return UNIFORM; }

<ACTIVE_MODE>"," { return COMMA; }
<ACTIVE_MODE>":" { return COLON; }
<ACTIVE_MODE>";" { return SEMICOLON; }
<ACTIVE_MODE>"(" { return L_PAREN; }
<ACTIVE_MODE>")" { return R_PAREN; }
<ACTIVE_MODE>"{" { return L_BRACE; }
<ACTIVE_MODE>"}" { return R_BRACE; }
<ACTIVE_MODE>"=" { return EQ; }

<ACTIVE_MODE>"void"  { return VOID; }
<ACTIVE_MODE>"int"   { return INT; }
<ACTIVE_MODE>"vec3"  { return VEC3; }
<ACTIVE_MODE>"ivec3" { return IVEC3; }
<ACTIVE_MODE>"mat4"  { return MAT4; }

<ACTIVE_MODE>\n  {  }
<ACTIVE_MODE>\r  {  }
<ACTIVE_MODE>\t  {  }
<ACTIVE_MODE>" " {  }

<ACTIVE_MODE>"//"[^\r\n]* {  }
<ACTIVE_MODE>"/*"         { BEGIN(MULTI_LINE_COMMENT); }
<MULTI_LINE_COMMENT>\n    {  }
<MULTI_LINE_COMMENT>.     {  }
<MULTI_LINE_COMMENT>"*/"  { BEGIN(ACTIVE_MODE); }

<ACTIVE_MODE>"#define ENTON_END" { BEGIN(INITIAL); }
<ACTIVE_MODE>"#"                 { BEGIN(PRE_PROCESSOR_DIRECTIVE); }
<PRE_PROCESSOR_DIRECTIVE>.*\\\n  {  }
<PRE_PROCESSOR_DIRECTIVE>.*\n    { BEGIN(ACTIVE_MODE); }

<ACTIVE_MODE>[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = string_from(yytext, yyleng); return IDENTIFIER; }
<ACTIVE_MODE>"\""[^"]*"\""          { yylval.string = string_from(yytext + 1, yyleng - 2); return STRING; }
<ACTIVE_MODE>[0-9]+                 { yylval.number = atoll(yytext + 1); return NUMBER; }

<ACTIVE_MODE>. {  }
%%