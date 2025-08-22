#ifndef ENTON_EXPRESSION_H
#define ENTON_EXPRESSION_H

#include <stdint.h>

#include "enton/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enton_expression_t enton_expression_none(void);
enton_expression_t enton_expression_packi(uint64_t expression_count, ...);
enton_expression_t enton_expression_packv(core_vector_t expressions);
enton_expression_t enton_expression_struct(core_vector_t expressions);
enton_expression_t enton_expression_ident(core_string_t identifier);
enton_expression_t enton_expression_string(core_string_t string);

void enton_expression_build(enton_expression_t expression);
void enton_expression_print(enton_expression_t expression, uint64_t indent_count, uint64_t indent_increment, uint8_t is_global, uint8_t is_first, uint8_t is_last);
void enton_expression_free(enton_expression_t expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ENTON_EXPRESSION_H
