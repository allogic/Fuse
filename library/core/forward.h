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

typedef struct set_entry_t {
  struct set_entry_t *next;
  uint8_t *key;
  uint64_t key_size;
} set_entry_t;

typedef struct set_t {
  set_entry_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t entry_count;
} set_t;

typedef struct map_record_t {
  struct map_record_t *next;
  uint8_t *key;
  uint64_t key_size;
  uint8_t *value;
  uint64_t value_size;
} map_record_t;

typedef struct map_t {
  map_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
} map_t;

typedef struct map_iter_t {
  map_record_t **table;
  map_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
} map_iter_t;

typedef struct vector_t {
  uint8_t *buffer;
  uint8_t *swap_buffer;
  uint64_t value_size;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  uint64_t buffer_count;
} vector_t;

#endif // CORE_FORWARD_H
