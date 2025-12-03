#ifndef EG_HEAP_H
#define EG_HEAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef BUILD_DEBUG
extern uint64_t g_heap_allocation_count;
extern uint64_t g_heap_rellocation_count;
extern uint64_t g_heap_allocated_bytes;
#endif // BUILD_DEBUG

void *eg_heap_alloc(uint64_t size, uint8_t zero_block, void const *reference);
void *eg_heap_realloc(void *block, uint64_t size);
void eg_heap_free(void *block);
void eg_heap_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_HEAP_H
