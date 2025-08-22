#ifndef CORE_HEAP_H
#define CORE_HEAP_H

#include <stdint.h>

#include <library/core/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint64_t g_core_heap_allocated_bytes;

void core_heap_prologue(void);
void *core_heap_alloc(uint64_t size);
void *core_heap_realloc(void *block, uint64_t size);
void core_heap_free(void *block);
void core_heap_epilogue(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_HEAP_H
