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

void context_push_preproc_decl(expression_t preproc_expression);
void context_push_struct_decl(expression_t struct_expression);
void context_push_layout_decl(expression_t layout_expression);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONTEXT_H
