#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

#include "enton/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void context_alloc(void);
void context_build(void);
void context_print(void);
void context_free(void);

void context_push_layout_decl(expression_t layout_expression);
void context_push_struct_decl(expression_t struct_expression);

void context_push_expression_vector(void);
void context_push_expression(expression_t expression);
core_vector_t context_pop_expression_vector(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONTEXT_H
