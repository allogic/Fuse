#include "enton/enton.h"
#include "enton/parser.h"

int32_t main(int32_t argc, char **argv) {
  core_heap_prologue();

  yyfilename = "C:\\Users\\burm\\Downloads\\Fuse\\shader\\test.glsl";

  FILE *file = fopen("C:\\Users\\burm\\Downloads\\Fuse\\shader\\test.glsl", "r");

  if (file) {
    context_alloc();

    yyrestart(file);
    yyparse();

    context_build();
    context_print();
    context_free();

    fclose(file);
  }

  core_heap_epilogue();

  return 0;
}
