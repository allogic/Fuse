#ifndef CORE_FORWARD_H
#define CORE_FORWARD_H

#include <stdint.h>

typedef struct file_t {
  char const *file_path;
  uint8_t *buffer;
  uint64_t buffer_size;
} file_t;

typedef struct string_t {
  char *buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
} string_t;

typedef struct vector_t {
  uint8_t *buffer;
  uint8_t *swap_buffer;
  uint64_t value_size;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  uint64_t buffer_count;
} vector_t;

#endif // CORE_FORWARD_H
