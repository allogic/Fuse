#include "stdio.h"
#include "string.h"

#include "engine/core/api.h"

#include "engine/parser/config.h"
#include "engine/parser/macros.h"
#include "engine/parser/context.h"

static core_vector_t s_parser_expressions = {0};

void parser_context_alloc(void) {
  s_parser_expressions = core_vector_alloc(sizeof(parser_expression_t));
}
void parser_context_build(void) {
}
void parser_context_print(void) {
  /*
  uint64_t expr_index = 0;
  uint64_t expr_count = vec_count(&s_exprs);
  while (expr_index < expr_count) {
    expr_t expr = *(expr_t *)vec_at(&s_exprs, expr_index);
    expr_print(expr, 0, 2, 1, expr_index == 0, expr_index == (expr_count - 1));
    expr_index++;
  }
  */
}
void parser_context_free(void) {
  /*
  uint64_t expr_index = 0;
  uint64_t expr_count = vec_count(&s_exprs);
  while (expr_index < expr_count) {
    expr_t expr = *(expr_t *)vec_at(&s_exprs, expr_index);
    expr_free(expr);
    expr_index++;
  }
  */

  core_vector_free(&s_parser_expressions);
}
