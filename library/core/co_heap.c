#include <library/lb_pch.h>

#include <library/core/co_heap.h>

#ifdef BUILD_DEBUG
uint64_t g_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG

void *heap_alloc(uint64_t size) {
#ifdef BUILD_DEBUG
  uint64_t new_size = sizeof(uint64_t) + size;

  uint64_t *new_block = (uint64_t *)malloc(new_size);

  g_heap_allocated_bytes += new_size;

  *new_block = new_size;

  new_block++;

  return new_block;
#else
  return malloc(size);
#endif // BUILD_DEBUG
}
void *heap_realloc(void *block, uint64_t size) {
#ifdef BUILD_DEBUG
  if (block) {
    uint64_t *old_block = (uint64_t *)block;

    old_block--;

    uint64_t old_size = *old_block;
    uint64_t new_size = sizeof(uint64_t) + size;

    g_heap_allocated_bytes -= old_size;

    uint64_t *new_block = (uint64_t *)realloc(old_block, new_size);

    g_heap_allocated_bytes += new_size;

    *new_block = new_size;

    new_block++;

    return new_block;
  } else {
    return heap_alloc(size);
  }
#else
  return realloc(block, size);
#endif // BUILD_DEBUG
}
void heap_free(void *block) {
#ifdef BUILD_DEBUG
  uint64_t *old_block = (uint64_t *)block;

  old_block--;

  uint64_t old_size = *old_block;

  g_heap_allocated_bytes -= old_size;

  free(old_block);
#else
  free(block);
#endif // BUILD_DEBUG
}
void heap_reset(void) {
#ifdef BUILD_DEBUG
  if (g_heap_allocated_bytes) {
    printf("%zu bytes leaked\n", g_heap_allocated_bytes);
  }

  g_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG
}
