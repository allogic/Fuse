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

      context_print();

      fclose(vertex_shader_file);
    }

    if (fragment_shader_file) {
      yyfilename = argv[3];

      yyrestart(fragment_shader_file);
      yyparse();

      context_print();

      fclose(fragment_shader_file);
    }

    context_build_pipeline_layouts(argv[4]);
    context_build_pipelines(argv[5]);
  } else if (strcmp("compute", argv[1]) == 0) {
    FILE *compute_shader_file = fopen(argv[2], "r");

    if (compute_shader_file) {
      yyfilename = argv[2];

      yyrestart(compute_shader_file);
      yyparse();

      context_print();

      fclose(compute_shader_file);
    }

    context_build(argv[3]);
  }

  context_free();

  heap_reset();

  return 0;
}
