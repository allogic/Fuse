#ifndef PARSER_EXPRESSION_H
#define PARSER_EXPRESSION_H

#include <stdint.h>

#include "engine/parser/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

parser_expression_t parser_expression_none(void);
parser_expression_t parser_expression_packi(uint64_t expr_count, ...);
parser_expression_t parser_expression_packv(vec_t exprs);
parser_expression_t parser_expression_rule(parser_expression_t pattern, parser_expression_t depend, parser_expression_t scope);
parser_expression_t parser_expression_call(parser_expression_t expression);
parser_expression_t parser_expression_var(parser_expression_t expression);
parser_expression_t parser_expression_if(parser_expression_t expression);
parser_expression_t parser_expression_copy(parser_expression_t left, parser_expression_t right);
parser_expression_t parser_expression_copy_add(parser_expression_t left, parser_expression_t right);
parser_expression_t parser_expression_copy_if(parser_expression_t left, parser_expression_t right);
parser_expression_t parser_expression_cond(parser_expression_t expression);
parser_expression_t parser_expression_comp(parser_expression_t left, parser_expression_t right);
parser_expression_t parser_expression_ident(str_t ident);
parser_expression_t parser_expression_string(str_t string);
parser_expression_t parser_expression_cond(parser_expression_t expression);
parser_expression_t parser_expression_if_block(parser_expression_t pack);
parser_expression_t parser_expression_if_else_block(parser_expression_t pack);
parser_expression_t parser_expression_mkdir(parser_expression_t expression);
parser_expression_t parser_expression_mkfile(parser_expression_t expression);
parser_expression_t parser_expression_rmdir(parser_expression_t expression);
parser_expression_t parser_expression_rmfile(parser_expression_t expression);
parser_expression_t parser_expression_printf(parser_expression_t expression);
parser_expression_t parser_expression_shell(parser_expression_t expression);

void parser_expression_build(parser_expression_t expression);
void parser_expression_print(parser_expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last);
void parser_expression_free(parser_expression_t expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PARSER_EXPRESSION_H
