#ifndef CORE_FORWARD_H
#define CORE_FORWARD_H

#include <stdint.h>

typedef struct core_string_t {
  char *buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
} core_string_t;

typedef struct core_vector_t {
  uint8_t *buffer;
  uint8_t *swap_buffer;
  uint64_t value_size;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  uint64_t buffer_count;
} core_vector_t;

#endif // CORE_FORWARD_H
