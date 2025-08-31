#include "stdio.h"
#include "string.h"

#include "library/core/api.h"

#include "enton/context.h"
#include "enton/expression.h"

static vector_t s_context_layout_decls = {0};
static vector_t s_context_struct_decls = {0};

static vector_t s_context_expression_stack = {0};

void context_alloc(void) {
  s_context_layout_decls = vector_alloc(sizeof(expression_t));
  s_context_struct_decls = vector_alloc(sizeof(expression_t));

  s_context_expression_stack = vector_alloc(sizeof(vector_t));
}
void context_build_pipeline_layouts(const char *output_file) {
  string_t pipeline_layouts = string_from_file(output_file);

  string_append(&pipeline_layouts, "-- Tha pipeline layouts representation\n");

  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)vector_at(&s_context_layout_decls, layout_expression_index);

    switch (layout_expression.expression_type) {
      case EXPRESSION_TYPE_LAYOUT_INPUT: {
        string_append(&pipeline_layouts, "Hello, Input World\n");
      } break;
      case EXPRESSION_TYPE_LAYOUT_UNIFORM: {
        string_append(&pipeline_layouts, "Hello, Uniform World\n");
      } break;
      default: {
      } break;
    }

    layout_expression_index++;
  }

  string_free(&pipeline_layouts);
}
void context_build_pipelines(const char *output_file) {
  string_t pipeline_layouts = string_from_file(output_file);

  string_append(&pipeline_layouts, "-- Tha pipeline representation\n");

  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)vector_at(&s_context_layout_decls, layout_expression_index);

    switch (layout_expression.expression_type) {
      case EXPRESSION_TYPE_LAYOUT_INPUT: {
        string_append(&pipeline_layouts, "Hello, Input World\n");
      } break;
      case EXPRESSION_TYPE_LAYOUT_UNIFORM: {
        string_append(&pipeline_layouts, "Hello, Uniform World\n");
      } break;
      default: {
      } break;
    }

    layout_expression_index++;
  }

  string_free(&pipeline_layouts);
}
void context_print(void) {
  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)vector_at(&s_context_layout_decls, layout_expression_index);

    expression_print(layout_expression, 0, 2, 1, layout_expression_index == 0, layout_expression_index == (layout_expression_count - 1));

    layout_expression_index++;
  }

  uint64_t struct_expression_index = 0;
  uint64_t struct_expression_count = vector_count(&s_context_struct_decls);
  while (struct_expression_index < struct_expression_count) {
    expression_t struct_expression = *(expression_t *)vector_at(&s_context_struct_decls, struct_expression_index);

    expression_print(struct_expression, 0, 2, 1, struct_expression_index == 0, struct_expression_index == (struct_expression_count - 1));

    struct_expression_index++;
  }
}
void context_reset(void) {
  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)vector_at(&s_context_layout_decls, layout_expression_index);

    expression_free(layout_expression);

    layout_expression_index++;
  }

  uint64_t struct_expression_index = 0;
  uint64_t struct_expression_count = vector_count(&s_context_struct_decls);
  while (struct_expression_index < struct_expression_count) {
    expression_t struct_expression = *(expression_t *)vector_at(&s_context_struct_decls, struct_expression_index);

    expression_free(struct_expression);

    struct_expression_index++;
  }

  vector_clear(&s_context_layout_decls);
  vector_clear(&s_context_struct_decls);

  vector_clear(&s_context_expression_stack);
}
void context_free(void) {
  uint64_t layout_expression_index = 0;
  uint64_t layout_expression_count = vector_count(&s_context_layout_decls);
  while (layout_expression_index < layout_expression_count) {
    expression_t layout_expression = *(expression_t *)vector_at(&s_context_layout_decls, layout_expression_index);

    expression_free(layout_expression);

    layout_expression_index++;
  }

  uint64_t struct_expression_index = 0;
  uint64_t struct_expression_count = vector_count(&s_context_struct_decls);
  while (struct_expression_index < struct_expression_count) {
    expression_t struct_expression = *(expression_t *)vector_at(&s_context_struct_decls, struct_expression_index);

    expression_free(struct_expression);

    struct_expression_index++;
  }

  vector_free(&s_context_layout_decls);
  vector_free(&s_context_struct_decls);

  vector_free(&s_context_expression_stack);
}

void context_push_layout_decl(expression_t layout_expression) {
  vector_push(&s_context_layout_decls, &layout_expression);
}
void context_push_struct_decl(expression_t struct_expression) {
  vector_push(&s_context_struct_decls, &struct_expression);
}

void context_push_expression_vector(void) {
  vector_t expressions = vector_alloc(sizeof(expression_t));

  vector_push(&s_context_expression_stack, &expressions);
}
void context_push_expression(expression_t expression) {
  vector_t *expressions = (vector_t *)vector_back(&s_context_expression_stack);

  vector_push(expressions, &expression);
}
vector_t context_pop_expression_vector(void) {
  vector_t expressions = {0};

  vector_pop(&s_context_expression_stack, &expressions);

  return expressions;
}
