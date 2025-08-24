#ifndef FORWARD_H
#define FORWARD_H

#include <stdint.h>

#include "library/core/api.h"

typedef enum expression_type_t {
  EXPRESSION_TYPE_NONE = 0,
  EXPRESSION_TYPE_PACK,
  EXPRESSION_TYPE_STRUCT,
  EXPRESSION_TYPE_IDENTIFIER,
  EXPRESSION_TYPE_STRING,
  EXPRESSION_TYPE_VERSION,
  EXPRESSION_TYPE_EXTENSION,
  EXPRESSION_TYPE_DATATYPE,
  EXPRESSION_TYPE_LAYOUT,
} expression_type_t;

typedef enum allocation_t {
  ALLOCATION_NONE = 0x0,
  ALLOCATION_IDENTIFIER = 0x1,
  ALLOCATION_STRING = 0x2,
  ALLOCATION_EXPRS = 0x4,
} allocation_t;

typedef enum datatype_t {
  DATATYPE_NONE = 0,
  DATATYPE_VOID = 1,
  DATATYPE_INT = 2,
  DATATYPE_VEC3 = 3,
  DATATYPE_IVEC3 = 4,
  DATATYPE_MAT4 = 5,
} datatype_t;

typedef struct expression_t {
  expression_type_t type;
  allocation_t allocation;
  core_string_t identifier;
  core_string_t string;
  int64_t number;
  core_vector_t expressions;
  datatype_t datatype;
} expression_t;

#endif // FORWARD_H
