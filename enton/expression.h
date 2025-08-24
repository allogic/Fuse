#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdint.h>

#include "enton/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

expression_t expression_none(void);
// expression_t expression_packi(uint64_t expression_count, ...);
// expression_t expression_packv(core_vector_t expressions);
expression_t expression_struct(datatype_t datatype, core_vector_t expressions);
expression_t expression_identifier(core_string_t identifier);
expression_t expression_string(core_string_t string);
expression_t expression_version(uint64_t version);
expression_t expression_extension(core_string_t extension);
expression_t expression_assign(expression_t left, expression_t right);

expression_t expression_layout_input(expression_t mods_expression, datatype_t datatype, core_string_t identifier);
expression_t expression_layout_uniform(expression_t mods_expression, expression_t struct_expression, core_string_t identifier);

void expression_build(expression_t expression);
void expression_print(expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last);
void expression_free(expression_t expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EXPRESSION_H
