#ifndef PARSER_CONTEXT_H
#define PARSER_CONTEXT_H

#include <stdint.h>

#include "engine/parser/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void parser_context_alloc(void);
void parser_context_build(void);
void parser_context_print(void);
void parser_context_free(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PARSER_CONTEXT_H
