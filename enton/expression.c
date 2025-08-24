#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "enton/expression.h"

expression_t expression_none(void) {
  expression_t none_expression;
  memset(&none_expression, 0, sizeof(expression_t));

  return none_expression;
}
expression_t expression_packi(uint64_t expression_count, ...) {
  expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(expression_t));

  pack_expression.type = EXPRESSION_TYPE_PACK;
  pack_expression.allocation = ALLOCATION_EXPRS;
  pack_expression.expressions = core_vector_alloc(sizeof(expression_t));

  uint64_t expression_index = 0;
  va_list args;
  va_start(args, expression_count);
  while (expression_index < expression_count) {
    expression_t expression = va_arg(args, expression_t);

    core_vector_push(&pack_expression.expressions, &expression);

    expression_index++;
  }
  va_end(args);

  return pack_expression;
}
expression_t expression_packv(core_vector_t expressions) {
  expression_t pack_expression;
  memset(&pack_expression, 0, sizeof(expression_t));

  pack_expression.type = EXPRESSION_TYPE_PACK;
  pack_expression.allocation = ALLOCATION_EXPRS;
  pack_expression.expressions = expressions;

  return pack_expression;
}
expression_t expression_struct(core_vector_t expressions) {
  expression_t struct_expression;
  memset(&struct_expression, 0, sizeof(expression_t));

  struct_expression.type = EXPRESSION_TYPE_STRUCT;
  struct_expression.allocation = ALLOCATION_EXPRS;
  struct_expression.expressions = expressions;

  return struct_expression;
}
expression_t expression_identifier(core_string_t identifier) {
  expression_t identifier_expression;
  memset(&identifier_expression, 0, sizeof(expression_t));

  identifier_expression.type = EXPRESSION_TYPE_IDENTIFIER;
  identifier_expression.allocation = ALLOCATION_IDENTIFIER;
  identifier_expression.identifier = identifier;

  return identifier_expression;
}
expression_t expression_string(core_string_t string) {
  expression_t string_expression;
  memset(&string_expression, 0, sizeof(expression_t));

  string_expression.type = EXPRESSION_TYPE_STRING;
  string_expression.allocation = ALLOCATION_STRING;
  string_expression.string = string;

  return string_expression;
}
expression_t expression_version(uint64_t version) {
  expression_t version_expression;
  memset(&version_expression, 0, sizeof(expression_t));

  version_expression.type = EXPRESSION_TYPE_VERSION;
  version_expression.allocation = ALLOCATION_NONE;
  version_expression.number = version;

  return version_expression;
}
expression_t expression_extension(core_string_t extension) {
  expression_t extension_expression;
  memset(&extension_expression, 0, sizeof(expression_t));

  extension_expression.type = EXPRESSION_TYPE_EXTENSION;
  extension_expression.allocation = ALLOCATION_STRING;
  extension_expression.string = extension;

  return extension_expression;
}
expression_t expression_assign(expression_t left, expression_t right) {
  expression_t assign_expression;
  memset(&assign_expression, 0, sizeof(expression_t));

  assign_expression.type = EXPRESSION_TYPE_EXTENSION;
  assign_expression.allocation = ALLOCATION_STRING;

  return assign_expression;
}

expression_t expression_layout_input(expression_t mods_expression, datatype_t datatype, core_string_t identifier) {
  expression_t layout_expression;
  memset(&layout_expression, 0, sizeof(expression_t));

  layout_expression.type = EXPRESSION_TYPE_LAYOUT;
  layout_expression.allocation = ALLOCATION_NONE;
  layout_expression.datatype = datatype;

  return layout_expression;
}
expression_t expression_layout_uniform(expression_t mods_expression, expression_t struct_expression, core_string_t identifier) {
  expression_t layout_expression;
  memset(&layout_expression, 0, sizeof(expression_t));

  layout_expression.type = EXPRESSION_TYPE_LAYOUT;
  layout_expression.allocation = ALLOCATION_NONE;
  layout_expression.datatype = datatype;

  return layout_expression;
}

void expression_build(expression_t expression) {
}
void expression_print(expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last) {
  uint64_t indent_index = 0;
  while (indent_index < indent_count) {
    printf(" ");
    indent_index++;
  }
  switch (expression.type) {
    case EXPRESSION_TYPE_NONE:
      printf("none\n");
      break;
    case EXPRESSION_TYPE_PACK:
      printf("pack\n");
      break;
    case EXPRESSION_TYPE_STRUCT:
      printf("struct\n");
      break;
    case EXPRESSION_TYPE_IDENTIFIER:
      printf("identifier %s\n", core_string_buffer(&expression.identifier));
      break;
    case EXPRESSION_TYPE_STRING:
      printf("string %s\n", core_string_buffer(&expression.string));
      break;
    default:
      break;
  }

  uint64_t expression_index = 0;
  uint64_t expression_count = core_vector_count(&expression.expressions);
  while (expression_index < expression_count) {
    expression_t sub_expr = *(expression_t *)core_vector_at(&expression.expressions, expression_index);

    expression_print(sub_expr, indent_count + indent_increment, indent_increment, 0, expression_index == 0, expression_index == (expression_count - 1));

    expression_index++;
  }

  if (is_global && (is_last == 0)) {
    printf("\n");
  }
}
void expression_free(expression_t expr) {
  if (expr.allocation & ALLOCATION_IDENTIFIER) {
    core_string_free(&expr.identifier);
  }

  if (expr.allocation & ALLOCATION_STRING) {
    core_string_free(&expr.string);
  }

  if (expr.allocation & ALLOCATION_EXPRS) {
    uint64_t expression_index = 0;
    uint64_t expression_count = core_vector_count(&expr.expressions);

    while (expression_index < expression_count) {
      expression_t sub_expressions = *(expression_t *)core_vector_at(&expr.expressions, expression_index);

      expression_free(sub_expressions);

      expression_index++;
    }

    core_vector_free(&expr.expressions);
  }

  memset(&expr, 0, sizeof(expression_t));
}
