#include "enton/enton.h"
#include "enton/parser.h"

int32_t main(int32_t argc, char **argv) {
  context_alloc();

  if (strcmp("render", argv[1]) == 0) {
    FILE *vertex_shader_file = fopen(argv[2], "r");
    FILE *fragment_shader_file = fopen(argv[3], "r");

    if (vertex_shader_file) {
      yyfilename = argv[2];

      yyrestart(vertex_shader_file);
      yyparse();

      fclose(vertex_shader_file);
    }

    if (fragment_shader_file) {
      yyfilename = argv[3];

      yyrestart(fragment_shader_file);
      yyparse();

      fclose(fragment_shader_file);
    }
  } else if (strcmp("compute", argv[1]) == 0) {
    FILE *compute_shader_file = fopen(argv[2], "r");

    if (compute_shader_file) {
      yyfilename = argv[2];

      yyrestart(compute_shader_file);
      yyparse();

      fclose(compute_shader_file);
    }
  }

  context_build();
  context_print();
  context_free();

  heap_reset();

  return 0;
}
