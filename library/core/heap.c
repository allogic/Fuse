#include <stdio.h>
#include <stdlib.h>

#include <library/core/heap.h>

#ifdef BUILD_DEBUG
uint64_t g_core_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG

void core_heap_prologue(void) {
  g_core_heap_allocated_bytes = 0;
}
void *core_heap_alloc(uint64_t size) {
#ifdef _DEBUG
  uint64_t new_size = sizeof(uint64_t) + size;
  uint64_t *new_block = (uint64_t *)malloc(new_size);

  g_core_heap_allocated_bytes += new_size;

  *new_block = new_size;
  new_block++;

  return new_block;
#else
  return malloc(size);
#endif // _DEBUG
}
void *core_heap_realloc(void *block, uint64_t size) {
#ifdef _DEBUG
  if (block) {
    uint64_t *old_block = (uint64_t *)block;

    old_block--;

    uint64_t old_size = *old_block;
    uint64_t new_size = sizeof(uint64_t) + size;

    g_core_heap_allocated_bytes -= old_size;

    uint64_t *new_block = (uint64_t *)realloc(old_block, new_size);

    g_core_heap_allocated_bytes += new_size;

    *new_block = new_size;
    new_block++;

    return new_block;
  } else {
    return core_heap_alloc(size);
  }
#else
  return realloc(block, size);
#endif // _DEBUG
}
void core_heap_free(void *block) {
#ifdef _DEBUG
  uint64_t *old_block = (uint64_t *)block;

  old_block--;

  uint64_t old_size = *old_block;

  g_core_heap_allocated_bytes -= old_size;

  free(old_block);
#else
  free(block);
#endif // _DEBUG
}
void core_heap_epilogue(void) {
#ifdef _DEBUG
  if (g_core_heap_allocated_bytes) {
    printf("%zu bytes not freed\n", g_core_heap_allocated_bytes);
  }
#endif // _DEBUG

  g_core_heap_allocated_bytes = 0;
}
