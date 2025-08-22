#ifndef PARSER_FORWARD_H
#define PARSER_FORWARD_H

#include <stdint.h>

#include "engine/core/api.h"

typedef enum parser_expression_type_t {
  PARSER_EXPRESSION_TYPE_NONE = 0,
  PARSER_EXPRESSION_TYPE_PACK,
  PARSER_EXPRESSION_TYPE_STRUCT,
  PARSER_EXPRESSION_TYPE_IDENT,
  PARSER_EXPRESSION_TYPE_STRING,
} parser_expression_type_t;

typedef enum parser_expression_alloc_t {
  PARSER_EXPRESSION_ALLOC_IDENT = 0x1,
  PARSER_EXPRESSION_ALLOC_STRING = 0x2,
  PARSER_EXPRESSION_ALLOC_EXPRS = 0x4,
} parser_expression_alloc_t;

typedef struct parser_expression_t {
  parser_expression_type_t type;
  parser_expression_alloc_t allocation;
  core_string_t ident;
  core_string_t string;
  core_vector_t expressions;
} parser_expression_t;

#endif // PARSER_FORWARD_H
