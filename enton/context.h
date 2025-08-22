#ifndef ENTON_CONTEXT_H
#define ENTON_CONTEXT_H

#include <stdint.h>

#include "enton/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void enton_context_alloc(void);
void enton_context_build(void);
void enton_context_print(void);
void enton_context_free(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ENTON_CONTEXT_H
