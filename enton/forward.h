#ifndef ENTON_FORWARD_H
#define ENTON_FORWARD_H

#include <stdint.h>

#include "library/core/api.h"

typedef enum enton_expression_type_t {
  ENTON_EXPRESSION_TYPE_NONE = 0,
  ENTON_EXPRESSION_TYPE_PACK,
  ENTON_EXPRESSION_TYPE_STRUCT,
  ENTON_EXPRESSION_TYPE_IDENT,
  ENTON_EXPRESSION_TYPE_STRING,
} enton_expression_type_t;

typedef enum enton_expression_alloc_t {
  ENTON_EXPRESSION_ALLOC_IDENT = 0x1,
  ENTON_EXPRESSION_ALLOC_STRING = 0x2,
  ENTON_EXPRESSION_ALLOC_EXPRS = 0x4,
} enton_expression_alloc_t;

typedef struct enton_expression_t {
  enton_expression_type_t type;
  enton_expression_alloc_t allocation;
  core_string_t ident;
  core_string_t string;
  core_vector_t expressions;
} enton_expression_t;

#endif // ENTON_FORWARD_H
