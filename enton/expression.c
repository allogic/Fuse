#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "enton/expression.h"

expression_t expression_none(void) {
  expression_t none_expression = {0};

  return none_expression;
}
expression_t expression_vector(vector_t vector) {
  expression_t pack_expression = {0};

  pack_expression.expression_type = EXPRESSION_TYPE_VECTOR;
  pack_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  pack_expression.vector = vector;

  return pack_expression;
}
expression_t expression_identifier(string_t identifier) {
  expression_t identifier_expression = {0};

  identifier_expression.expression_type = EXPRESSION_TYPE_IDENTIFIER;
  identifier_expression.allocation_type = ALLOCATION_TYPE_STRING;
  identifier_expression.string = identifier;

  return identifier_expression;
}
expression_t expression_custom_type(string_t identifier) {
  expression_t type_expression = {0};

  type_expression.expression_type = EXPRESSION_TYPE_CUSTOM_TYPE;
  type_expression.allocation_type = ALLOCATION_TYPE_STRING;
  type_expression.string = identifier;

  return type_expression;
}
expression_t expression_string(string_t string) {
  expression_t string_expression = {0};

  string_expression.expression_type = EXPRESSION_TYPE_STRING;
  string_expression.allocation_type = ALLOCATION_TYPE_STRING;
  string_expression.string = string;

  return string_expression;
}
expression_t expression_number(int64_t number) {
  expression_t number_expression = {0};

  number_expression.expression_type = EXPRESSION_TYPE_NUMBER;
  number_expression.allocation_type = ALLOCATION_TYPE_NONE;
  number_expression.number = number;

  return number_expression;
}
expression_t expression_primitive_type(primitive_type_t primitive_type) {
  expression_t extension_expression = {0};

  extension_expression.expression_type = EXPRESSION_TYPE_PRIMITIVE_TYPE;
  extension_expression.allocation_type = ALLOCATION_TYPE_NONE;
  extension_expression.primitive_type = primitive_type;

  return extension_expression;
}

expression_t expression_assignment(expression_t left_expression, expression_t right_expression) {
  expression_t assignment_expression = {0};

  assignment_expression.expression_type = EXPRESSION_TYPE_ASSIGNMENT;
  assignment_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  assignment_expression.vector = vector_alloc(sizeof(expression_t));

  vector_push(&assignment_expression.vector, &left_expression);
  vector_push(&assignment_expression.vector, &right_expression);

  return assignment_expression;
}
expression_t expression_layout_input(expression_t type_expression, expression_t identifier_expression, expression_t modifier_expression) {
  expression_t input_expression = {0};

  input_expression.expression_type = EXPRESSION_TYPE_LAYOUT_INPUT;
  input_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  input_expression.vector = vector_alloc(sizeof(expression_t));

  vector_push(&input_expression.vector, &type_expression);
  vector_push(&input_expression.vector, &identifier_expression);
  vector_push(&input_expression.vector, &modifier_expression);

  return input_expression;
}
expression_t expression_layout_uniform(expression_t type_expression, expression_t identifier_expression, expression_t modifier_expression) {
  expression_t uniform_expression = {0};

  uniform_expression.expression_type = EXPRESSION_TYPE_LAYOUT_UNIFORM;
  uniform_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  uniform_expression.vector = vector_alloc(sizeof(expression_t));

  vector_push(&uniform_expression.vector, &type_expression);
  vector_push(&uniform_expression.vector, &identifier_expression);
  vector_push(&uniform_expression.vector, &modifier_expression);

  return uniform_expression;
}
expression_t expression_struct(expression_t identifier_expression, expression_t struct_member_expressions) {
  expression_t struct_expression = {0};

  struct_expression.expression_type = EXPRESSION_TYPE_STRUCT;
  struct_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  struct_expression.vector = vector_alloc(sizeof(expression_t));

  vector_push(&struct_expression.vector, &identifier_expression);
  vector_push(&struct_expression.vector, &struct_member_expressions);

  return struct_expression;
}
expression_t expression_struct_member(expression_t primitive_type_expression, expression_t identifier_expression) {
  expression_t struct_member_expression = {0};

  struct_member_expression.expression_type = EXPRESSION_TYPE_STRUCT_MEMBER;
  struct_member_expression.allocation_type = ALLOCATION_TYPE_VECTOR;
  struct_member_expression.vector = vector_alloc(sizeof(expression_t));

  vector_push(&struct_member_expression.vector, &primitive_type_expression);
  vector_push(&struct_member_expression.vector, &identifier_expression);

  return struct_member_expression;
}

void expression_build(expression_t expression) {
}
void expression_print(expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last) {
  uint64_t indent_index = 0;
  while (indent_index < indent_count) {
    printf(" ");

    indent_index++;
  }

  switch (expression.expression_type) {
    case EXPRESSION_TYPE_NONE:
      printf("none\n");
      break;
    case EXPRESSION_TYPE_VECTOR:
      printf("vector\n");
      break;
    case EXPRESSION_TYPE_IDENTIFIER:
      printf("identifier: %s\n", string_buffer(&expression.string));
      break;
    case EXPRESSION_TYPE_CUSTOM_TYPE:
      printf("custom type: %s\n", string_buffer(&expression.string));
      break;
    case EXPRESSION_TYPE_STRING:
      printf("string: %s\n", string_buffer(&expression.string));
      break;
    case EXPRESSION_TYPE_NUMBER:
      printf("number: %lld\n", expression.number);
      break;
    case EXPRESSION_TYPE_PRIMITIVE_TYPE:
      printf("primitive type: ");
      switch (expression.primitive_type) {
        case PRIMITIVE_TYPE_VOID:
          printf("void");
          break;
        case PRIMITIVE_TYPE_INT:
          printf("int");
          break;
        case PRIMITIVE_TYPE_VEC3:
          printf("vec3");
          break;
        case PRIMITIVE_TYPE_IVEC3:
          printf("ivec3");
          break;
        case PRIMITIVE_TYPE_MAT4:
          printf("mat4");
          break;
        default:
          break;
      }
      printf("\n");
      break;
    case EXPRESSION_TYPE_ASSIGNMENT:
      printf("assignment\n");
      break;
    case EXPRESSION_TYPE_LAYOUT_INPUT:
      printf("layout input\n");
      break;
    case EXPRESSION_TYPE_LAYOUT_UNIFORM:
      printf("layout uniform\n");
      break;
    case EXPRESSION_TYPE_STRUCT:
      printf("struct\n");
      break;
    case EXPRESSION_TYPE_STRUCT_MEMBER:
      printf("struct member\n");
      break;
    default:
      break;
  }

  uint64_t expression_index = 0;
  uint64_t expression_count = vector_count(&expression.vector);
  while (expression_index < expression_count) {
    expression_t sub_expr = *(expression_t *)vector_at(&expression.vector, expression_index);

    expression_print(sub_expr, indent_count + indent_increment, indent_increment, 0, expression_index == 0, expression_index == (expression_count - 1));

    expression_index++;
  }

  if (is_global && (is_last == 0)) {
    printf("\n");
  }
}
void expression_free(expression_t expr) {
  if (expr.allocation_type & ALLOCATION_TYPE_STRING) {
    string_free(&expr.string);
  }

  if (expr.allocation_type & ALLOCATION_TYPE_VECTOR) {
    uint64_t expression_index = 0;
    uint64_t expression_count = vector_count(&expr.vector);
    while (expression_index < expression_count) {
      expression_t sub_expressions = *(expression_t *)vector_at(&expr.vector, expression_index);

      expression_free(sub_expressions);

      expression_index++;
    }

    vector_free(&expr.vector);
  }
}
