#include "stdio.h"
#include "string.h"

#include "library/core/api.h"

#include "enton/context.h"
#include "enton/expression.h"

static core_vector_t s_context_preproc_decls = {0};
static core_vector_t s_context_struct_decls = {0};
static core_vector_t s_context_layout_decls = {0};

static core_vector_t s_context_expression_stack = {0};

void context_alloc(void) {
  s_context_preproc_decls = core_vector_alloc(sizeof(expression_t));
  s_context_struct_decls = core_vector_alloc(sizeof(expression_t));
  s_context_layout_decls = core_vector_alloc(sizeof(expression_t));

  s_context_expression_stack = core_vector_alloc(sizeof(core_vector_t));
}
void context_build(void) {
}
void context_print(void) {
  uint64_t preproc_expression_index = 0;
  uint64_t preproc_expression_count = core_vector_count(&s_context_preproc_decls);
  while (preproc_expression_index < preproc_expression_count) {
    expression_t preproc_expression = *(expression_t *)core_vector_at(&s_context_preproc_decls, preproc_expression_index);

    expression_print(preproc_expression, 0, 2, 1, preproc_expression_index == 0, preproc_expression_index == (preproc_expression_count - 1));

    preproc_expression_index++;
  }

  uint64_t struct_expression_index = 0;
  uint64_t struct_expression_count = core_vector_count(&s_context_struct_decls);
  while (struct_expression_index < struct_expression_count) {
    expression_t struct_expression = *(expression_t *)core_vector_at(&s_context_struct_decls, struct_expression_index);

    expression_print(struct_expression, 0, 2, 1, struct_expression_index == 0, struct_expression_index == (struct_expression_count - 1));

    struct_expression_index++;
  }

  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = core_vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)core_vector_at(&s_context_layout_decls, layout_expression_index);

    expression_print(layout_expression, 0, 2, 1, layout_expression_index == 0, layout_expression_index == (layout_expression_count - 1));

    layout_expression_index++;
  }
}
void context_free(void) {
  uint64_t preproc_expression_index = 0;
  uint64_t preproc_expression_count = core_vector_count(&s_context_preproc_decls);
  while (preproc_expression_index < preproc_expression_count) {
    expression_t preproc_expression = *(expression_t *)core_vector_at(&s_context_preproc_decls, preproc_expression_index);

    expression_free(preproc_expression);

    preproc_expression_index++;
  }

  uint64_t struct_expression_index = 0;
  uint64_t struct_expression_count = core_vector_count(&s_context_struct_decls);
  while (struct_expression_index < struct_expression_count) {
    expression_t struct_expression = *(expression_t *)core_vector_at(&s_context_struct_decls, struct_expression_index);

    expression_free(struct_expression);

    struct_expression_index++;
  }

  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = core_vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)core_vector_at(&s_context_layout_decls, layout_expression_index);

    expression_free(layout_expression);

    layout_expression_index++;
  }

  core_vector_free(&s_context_preproc_decls);
  core_vector_free(&s_context_struct_decls);
  core_vector_free(&s_context_layout_decls);

  core_vector_free(&s_context_expression_stack);
}

void context_push_preproc_decl(expression_t preproc_expression) {
  core_vector_push(&s_context_preproc_decls, &preproc_expression);
}
void context_push_struct_decl(expression_t struct_expression) {
  core_vector_push(&s_context_struct_decls, &struct_expression);
}
void context_push_layout_decl(expression_t layout_expression) {
  core_vector_push(&s_context_layout_decls, &layout_expression);
}

void context_push_expression_vector(void) {
  core_vector_t expressions = core_vector_alloc(sizeof(expression_t));

  core_vector_push(&s_context_expression_stack, &expressions);
}
void context_push_expression(expression_t expression) {
  core_vector_t *expressions = (core_vector_t *)core_vector_back(&s_context_expression_stack);

  core_vector_push(expressions, &expression);
}
core_vector_t context_pop_expression_vector(void) {
  core_vector_t expressions = {0};

  core_vector_pop(&s_context_expression_stack, &expressions);

  return expressions;
}
