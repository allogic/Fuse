#include "stdio.h"
#include "string.h"

#include "library/core/api.h"

#include "enton/config.h"
#include "enton/macros.h"
#include "enton/context.h"
#include "enton/expression.h"

static core_vector_t s_enton_expressions = {0};

void enton_context_alloc(void) {
  s_enton_expressions = core_vector_alloc(sizeof(enton_expression_t));
}
void enton_context_build(void) {
}
void enton_context_print(void) {
  uint64_t expression_index = 0;
  uint64_t expression_count = core_vector_count(&s_enton_expressions);
  while (expression_index < expression_count) {
    enton_expression_t expression = *(enton_expression_t *)core_vector_at(&s_enton_expressions, expression_index);

    enton_expression_print(expression, 0, 2, 1, expression_index == 0, expression_index == (expression_count - 1));

    expression_index++;
  }
}
void enton_context_free(void) {
  uint64_t expression_index = 0;
  uint64_t expression_count = core_vector_count(&s_enton_expressions);
  while (expression_index < expression_count) {
    enton_expression_t expression = *(enton_expression_t *)core_vector_at(&s_enton_expressions, expression_index);

    enton_expression_free(expression);

    expression_index++;
  }

  core_vector_free(&s_enton_expressions);
}
