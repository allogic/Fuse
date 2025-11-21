#include <library/core/lb_pch.h>
#include <library/core/lb_heap.h>

#ifdef BUILD_DEBUG
uint64_t g_heap_allocation_count = 0;
uint64_t g_heap_rellocation_count = 0;
uint64_t g_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG

void *lb_heap_alloc(uint64_t size, uint8_t zero_block, void const *reference) {
#ifdef BUILD_DEBUG
  uint64_t new_size = sizeof(uint64_t) + size;

  uint64_t *new_block = (uint64_t *)malloc(new_size);

  g_heap_allocation_count += 1;
  g_heap_allocated_bytes += new_size;

  *new_block = new_size;

  new_block++;

  if (zero_block) {
    memset(new_block, 0, size);
  }

  if (reference) {
    memcpy(new_block, reference, size);
  }

  printf("Allocation(%llu) Size(%llu) HeapSize(%llu)\n", g_heap_allocation_count, size, g_heap_allocated_bytes);

  return new_block;
#else
  uint64_t *new_block = malloc(size);

  if (zero_block) {
    memset(new_block, 0, size);
  }

  if (reference) {
    memcpy(new_block, reference, size);
  }

  return new_block;
#endif // BUILD_DEBUG
}
void *lb_heap_realloc(void *block, uint64_t size) {
#ifdef BUILD_DEBUG
  if (block) {
    uint64_t *old_block = (uint64_t *)block;

    old_block--;

    uint64_t old_size = *old_block;
    uint64_t new_size = sizeof(uint64_t) + size;

    g_heap_allocated_bytes -= old_size;

    uint64_t *new_block = (uint64_t *)realloc(old_block, new_size);

    g_heap_rellocation_count += 1;
    g_heap_allocated_bytes += new_size;

    *new_block = new_size;

    new_block++;

    printf("Rellocation(%llu) Size(%llu) HeapSize(%llu)\n", g_heap_allocation_count, size, g_heap_allocated_bytes);

    return new_block;
  } else {
    return lb_heap_alloc(size, 0, 0);
  }
#else
  return realloc(block, size);
#endif // BUILD_DEBUG
}
void lb_heap_free(void *block) {
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
void lb_heap_reset(void) {
#ifdef BUILD_DEBUG
  if (g_heap_allocated_bytes) {
    printf("%zu bytes leaked\n", g_heap_allocated_bytes);
  }

  g_heap_allocated_bytes = 0;
#endif // BUILD_DEBUG
}
