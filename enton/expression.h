#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdint.h>

#include "enton/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

expression_t expression_none(void);
expression_t expression_vector(core_vector_t vector);
expression_t expression_identifier(core_string_t identifier);
expression_t expression_custom_type(core_string_t identifier);
expression_t expression_string(core_string_t string);
expression_t expression_number(int64_t number);
expression_t expression_primitive_type(primitive_type_t primitive_type);

expression_t expression_assignment(expression_t left_expression, expression_t right_expression);
expression_t expression_layout_input(expression_t type_expression, expression_t identifier_expression, expression_t modifier_expression);
expression_t expression_layout_uniform(expression_t type_expression, expression_t identifier_expression, expression_t modifier_expression);
expression_t expression_struct(expression_t identifier_expression, expression_t struct_member_expressions);
expression_t expression_struct_member(expression_t primitive_type_expression, expression_t identifier_expression);

void expression_build(expression_t expression);
void expression_print(expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last);
void expression_free(expression_t expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EXPRESSION_H
