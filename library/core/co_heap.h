#ifndef CO_HEAP_H
#define CO_HEAP_H

#include <stdint.h>

#include <library/core/co_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef BUILD_DEBUG
extern uint64_t g_heap_allocated_bytes;
#endif // BUILD_DEBUG

void *heap_alloc(uint64_t size);
void *heap_realloc(void *block, uint64_t size);
void heap_free(void *block);
void heap_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CO_HEAP_H
