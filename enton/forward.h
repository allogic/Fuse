#ifndef FORWARD_H
#define FORWARD_H

#include <stdint.h>

#include "library/core/api.h"

typedef enum expression_type_t {
  EXPRESSION_TYPE_NONE = 0,
  EXPRESSION_TYPE_VECTOR,
  EXPRESSION_TYPE_IDENTIFIER,
  EXPRESSION_TYPE_CUSTOM_TYPE,
  EXPRESSION_TYPE_STRING,
  EXPRESSION_TYPE_NUMBER,
  EXPRESSION_TYPE_PRIMITIVE_TYPE,
  EXPRESSION_TYPE_ASSIGNMENT,
  EXPRESSION_TYPE_LAYOUT_INPUT,
  EXPRESSION_TYPE_LAYOUT_UNIFORM,
  EXPRESSION_TYPE_STRUCT,
  EXPRESSION_TYPE_STRUCT_MEMBER,
} expression_type_t;

typedef enum allocation_type_t {
  ALLOCATION_TYPE_NONE = 0x0,
  ALLOCATION_TYPE_STRING = 0x1,
  ALLOCATION_TYPE_VECTOR = 0x2,
} allocation_type_t;

typedef enum primitive_type_t {
  PRIMITIVE_TYPE_NONE = 0,
  PRIMITIVE_TYPE_VOID = 1,
  PRIMITIVE_TYPE_INT = 2,
  PRIMITIVE_TYPE_VEC3 = 3,
  PRIMITIVE_TYPE_IVEC3 = 4,
  PRIMITIVE_TYPE_MAT4 = 5,
} primitive_type_t;

typedef struct expression_t {
  expression_type_t expression_type;
  allocation_type_t allocation_type;
  primitive_type_t primitive_type;
  string_t string;
  vector_t vector;
  int64_t number;
} expression_t;

#endif // FORWARD_H
