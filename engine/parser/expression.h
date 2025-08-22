#ifndef PARSER_EXPRESSION_H
#define PARSER_EXPRESSION_H

#include <stdint.h>

#include "engine/parser/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

parser_expression_t parser_expression_none(void);
parser_expression_t parser_expression_packi(uint64_t expression_count, ...);
parser_expression_t parser_expression_packv(core_vector_t expressions);
parser_expression_t parser_expression_struct(core_vector_t expressions);
parser_expression_t parser_expression_ident(core_string_t identifier);
parser_expression_t parser_expression_string(core_string_t string);

void parser_expression_build(parser_expression_t expression);
void parser_expression_print(parser_expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last);
void parser_expression_free(parser_expression_t expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PARSER_EXPRESSION_H
