#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "enton/expression.h"

enton_expression_t enton_expression_none(void) {
  enton_expression_t none_expression;
  memset(&none_expression, 0, sizeof(enton_expression_t));

  return none_expression;
}
enton_expression_t enton_expression_packi(uint64_t expression_count, ...) {
  enton_expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(enton_expression_t));

  pack_expression.type = ENTON_EXPRESSION_TYPE_PACK;
  pack_expression.allocation = ENTON_EXPRESSION_ALLOC_EXPRS;
  pack_expression.expressions = core_vector_alloc(sizeof(enton_expression_t));

  uint64_t expression_index = 0;
  va_list args;
  va_start(args, expression_count);
  while (expression_index < expression_count) {
    enton_expression_t expression = va_arg(args, enton_expression_t);

    core_vector_push(&pack_expression.expressions, &expression);

    expression_index++;
  }
  va_end(args);

  return pack_expression;
}
enton_expression_t enton_expression_packv(core_vector_t expressions) {
  enton_expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(enton_expression_t));

  pack_expression.type = ENTON_EXPRESSION_TYPE_PACK;
  pack_expression.allocation = ENTON_EXPRESSION_ALLOC_EXPRS;
  pack_expression.expressions = expressions;

  return pack_expression;
}
enton_expression_t enton_expression_struct(core_vector_t expressions) {
  enton_expression_t struct_expression;
  memset(&struct_expression, 0, sizeof(enton_expression_t));

  struct_expression.type = ENTON_EXPRESSION_TYPE_STRUCT;
  struct_expression.allocation = ENTON_EXPRESSION_ALLOC_EXPRS;
  struct_expression.expressions = expressions;

  return struct_expression;
}
enton_expression_t enton_expression_ident(core_string_t identifier) {
  enton_expression_t ident_expression;
  memset(&ident_expression, 0, sizeof(enton_expression_t));

  ident_expression.type = ENTON_EXPRESSION_TYPE_IDENT;
  ident_expression.allocation = ENTON_EXPRESSION_ALLOC_IDENT;
  ident_expression.ident = identifier;

  return ident_expression;
}
enton_expression_t enton_expression_string(core_string_t string) {
  enton_expression_t string_expression;
  memset(&string_expression, 0, sizeof(enton_expression_t));

  string_expression.type = ENTON_EXPRESSION_TYPE_STRING;
  string_expression.allocation = ENTON_EXPRESSION_ALLOC_STRING;
  string_expression.string = string;

  return string_expression;
}

void enton_expression_build(enton_expression_t expression) {
}
void enton_expression_print(enton_expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last) {
  uint64_t indent_index = 0;
  while (indent_index < indent_count) {
    printf(" ");
    indent_index++;
  }
  switch (expression.type) {
    case ENTON_EXPRESSION_TYPE_NONE:
      printf("none\n");
      break;
    case ENTON_EXPRESSION_TYPE_PACK:
      printf("pack\n");
      break;
    case ENTON_EXPRESSION_TYPE_STRUCT:
      printf("struct\n");
      break;
    case ENTON_EXPRESSION_TYPE_IDENT:
      printf("ident %s\n", core_string_buffer(&expression.ident));
      break;
    case ENTON_EXPRESSION_TYPE_STRING:
      printf("string %s\n", core_string_buffer(&expression.string));
      break;
    default:
      break;
  }

  uint64_t expression_index = 0;
  uint64_t expression_count = core_vector_count(&expression.expressions);
  while (expression_index < expression_count) {
    enton_expression_t sub_expr = *(enton_expression_t *)core_vector_at(&expression.expressions, expression_index);

    enton_expression_print(sub_expr, indent_count + indent_increment, indent_increment, 0, expression_index == 0, expression_index == (expression_count - 1));

    expression_index++;
  }

  if (is_global && (is_last == 0)) {
    printf("\n");
  }
}
void enton_expression_free(enton_expression_t expr) {
  if (expr.allocation & ENTON_EXPRESSION_ALLOC_IDENT) {
    core_string_free(&expr.ident);
  }

  if (expr.allocation & ENTON_EXPRESSION_ALLOC_STRING) {
    core_string_free(&expr.string);
  }

  if (expr.allocation & ENTON_EXPRESSION_ALLOC_EXPRS) {
    uint64_t expression_index = 0;
    uint64_t expression_count = core_vector_count(&expr.expressions);

    while (expression_index < expression_count) {
      enton_expression_t sub_expressions = *(enton_expression_t *)core_vector_at(&expr.expressions, expression_index);

      enton_expression_free(sub_expressions);

      expression_index++;
    }

    core_vector_free(&expr.expressions);
  }

  memset(&expr, 0, sizeof(enton_expression_t));
}
