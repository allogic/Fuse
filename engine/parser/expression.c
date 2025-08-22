#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "engine/parser/expression.h"

parser_expression_t parser_expression_none(void) {
  parser_expression_t none_expression;
  memset(&none_expression, 0, sizeof(parser_expression_t));

  return none_expression;
}
parser_expression_t parser_expression_packi(uint64_t expression_count, ...) {
  parser_expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(parser_expression_t));

  pack_expression.type = PARSER_EXPRESSION_TYPE_PACK;
  pack_expression.allocation = PARSER_EXPRESSION_ALLOC_EXPRS;
  pack_expression.expressions = core_vector_alloc(sizeof(parser_expression_t));

  uint64_t expression_index = 0;
  va_list args;
  va_start(args, expression_count);
  while (expression_index < expression_count) {
    parser_expression_t expression = va_arg(args, parser_expression_t);

    core_vector_push(&pack_expression.expressions, &expression);

    expression_index++;
  }
  va_end(args);

  return pack_expression;
}
parser_expression_t parser_expression_packv(core_vector_t expressions) {
  parser_expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(parser_expression_t));

  pack_expression.type = PARSER_EXPRESSION_TYPE_PACK;
  pack_expression.allocation = PARSER_EXPRESSION_ALLOC_EXPRS;
  pack_expression.expressions = expressions;

  return pack_expression;
}
parser_expression_t parser_expression_struct(core_vector_t expressions) {
  parser_expression_t struct_expression;
  memset(&struct_expression, 0, sizeof(parser_expression_t));

  struct_expression.type = PARSER_EXPRESSION_TYPE_STRUCT;
  struct_expression.allocation = PARSER_EXPRESSION_ALLOC_EXPRS;
  struct_expression.expressions = expressions;

  return struct_expression;
}
parser_expression_t parser_expression_ident(core_string_t identifier) {
  parser_expression_t ident_expression;
  memset(&ident_expression, 0, sizeof(parser_expression_t));

  ident_expression.type = PARSER_EXPRESSION_TYPE_IDENT;
  ident_expression.allocation = PARSER_EXPRESSION_ALLOC_IDENT;
  ident_expression.ident = identifier;

  return ident_expression;
}
parser_expression_t parser_expression_string(core_string_t string) {
  parser_expression_t string_expression;
  memset(&string_expression, 0, sizeof(parser_expression_t));

  string_expression.type = PARSER_EXPRESSION_TYPE_STRING;
  string_expression.allocation = PARSER_EXPRESSION_ALLOC_STRING;
  string_expression.string = string;

  return string_expression;
}

void parser_expression_build(parser_expression_t expression) {
}
void parser_expression_print(parser_expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last) {
  uint64_t indent_index = 0;
  while (indent_index < indent_count) {
    printf(" ");
    indent_index++;
  }
  switch (expression.type) {
    case PARSER_EXPRESSION_TYPE_NONE:
      printf("none\n");
      break;
    case PARSER_EXPRESSION_TYPE_PACK:
      printf("pack\n");
      break;
    case PARSER_EXPRESSION_TYPE_STRUCT:
      printf("struct\n");
      break;
    case PARSER_EXPRESSION_TYPE_IDENT:
      printf("ident %s\n", core_string_buffer(&expression.ident));
      break;
    case PARSER_EXPRESSION_TYPE_STRING:
      printf("string %s\n", core_string_buffer(&expression.string));
      break;
    default:
      break;
  }

  uint64_t expr_index = 0;
  uint64_t expr_count = core_vector_count(&expression.expressions);
  while (expr_index < expr_count) {
    parser_expression_t sub_expr = *(parser_expression_t *)core_vector_at(&expression.expressions, expr_index);

    parser_expression_print(sub_expr, indent_count + indent_increment, indent_increment, 0, expr_index == 0, expr_index == (expr_count - 1));

    expr_index++;
  }

  if (is_global && (is_last == 0)) {
    printf("\n");
  }
}
void parser_expression_free(parser_expression_t expr) {
  if (expr.allocation & PARSER_EXPRESSION_ALLOC_IDENT) {
    core_string_free(&expr.ident);
  }

  if (expr.allocation & PARSER_EXPRESSION_ALLOC_STRING) {
    core_string_free(&expr.string);
  }

  if (expr.allocation & PARSER_EXPRESSION_ALLOC_EXPRS) {
    uint64_t expr_index = 0;
    uint64_t expr_count = core_vector_count(&expr.expressions);

    while (expr_index < expr_count) {
      parser_expression_t sub_expr = *(parser_expression_t *)core_vector_at(&expr.expressions, expr_index);

      parser_expression_free(sub_expr);

      expr_index++;
    }

    core_vector_free(&expr.expressions);
  }

  memset(&expr, 0, sizeof(parser_expression_t));
}
