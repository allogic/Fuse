#ifdef _WIN32
#  include <io.h>

#  define fileno _fileno
#  define isatty _isatty
#endif // _WIN32

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "library/core/api.h"

#include "enton/config.h"
#include "enton/context.h"
#include "enton/expression.h"

extern char const *g_current_filename;
extern char *yytext;

extern int32_t g_line_number;
extern int32_t g_column_number;
extern int32_t yyleng;

int32_t yyerror(char const *msg, ...);
int32_t yywrap(void);
int32_t yylex(void);

void yyrestart(FILE *input_file);
