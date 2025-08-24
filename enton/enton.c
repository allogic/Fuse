#include "enton/enton.h"
#include "enton/parser.h"

int32_t main(int32_t argc, char **argv) {
  core_heap_prologue();

  g_current_filename = ROOT_DIR "/shader/chunk/renderer.vert";

  g_line_number = 1;
  g_column_number = 1;

  FILE *file = fopen(ROOT_DIR "/shader/chunk/renderer.vert", "r");

  if (file) {
    context_alloc();

    yyrestart(file);
    yyparse();

    context_build();
    context_print();
    context_free();

    fclose(file);
  }

  printf("DONE\n");

  core_heap_epilogue();

  return 0;
}
