#include "enton/prologue.inl"

char const *g_current_filename;

int32_t g_line_number;
int32_t g_column_number;

int32_t yyerror(char const *fmt, ...) {
  static char fmt_buffer[ENTON_LOG_FMT_BUFFER_SIZE];

  va_list args;
  va_start(args, fmt);
  vsnprintf(fmt_buffer, ENTON_LOG_FMT_BUFFER_SIZE, fmt, args);
  printf("%s:%d:%d: %s\n", g_current_filename, yylloc.first_line, yylloc.first_column, fmt_buffer);
  va_end(args);

  return 1;
}
int32_t yywrap(void) {
  return 1;
}
