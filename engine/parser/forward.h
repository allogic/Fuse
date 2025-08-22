#ifndef PARSER_FORWARD_H
#define PARSER_FORWARD_H

#include <stdint.h>

#include "engine/core/api.h"

typedef enum parser_expression_type_t {
  EXPR_TYPE_NONE = 0,
  EXPR_TYPE_PACK,
  EXPR_TYPE_RULE,
  EXPR_TYPE_CALL,
  EXPR_TYPE_VAR,
  EXPR_TYPE_IF,
  EXPR_TYPE_COPY,
  EXPR_TYPE_COPY_ADD,
  EXPR_TYPE_COPY_IF,
  EXPR_TYPE_IDENT,
  EXPR_TYPE_STRING,
  EXPR_TYPE_COND,
  EXPR_TYPE_COMP,
  EXPR_TYPE_IF_BLOCK,
  EXPR_TYPE_IF_ELSE_BLOCK,
  EXPR_TYPE_MKDIR,
  EXPR_TYPE_MKFILE,
  EXPR_TYPE_RMDIR,
  EXPR_TYPE_RMFILE,
  EXPR_TYPE_PRINTF,
  EXPR_TYPE_SHELL,
} parser_expression_type_t;

typedef enum parser_expression_alloc_t {
  EXPR_ALLOC_IDENT = 0x1,
  EXPR_ALLOC_STRING = 0x2,
  EXPR_ALLOC_EXPRS = 0x4,
} parser_expression_alloc_t;

typedef struct parser_expression_t {
  parser_expression_type_t type;
  parser_expression_alloc_t allocation;
  core_string_t ident;
  core_string_t string;
  core_vector_t expressions;
} parser_expression_t;

#endif // PARSER_FORWARD_H
