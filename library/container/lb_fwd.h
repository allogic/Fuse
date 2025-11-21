#ifndef LB_CONTAINER_FWD_H
#define LB_CONTAINER_FWD_H

typedef struct lb_string_t {
  char *buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
} lb_string_t;

typedef struct lb_set_record_t {
  struct lb_set_record_t *next;
  uint8_t *key;
  uint64_t key_size;
} lb_set_record_t;

typedef struct lb_set_t {
  struct lb_set_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
} lb_set_t;

typedef struct lb_map_record_t {
  struct lb_map_record_t *next;
  uint8_t *key;
  uint64_t key_size;
  uint8_t *value;
  uint64_t value_size;
} lb_map_record_t;

typedef struct lb_map_t {
  struct lb_map_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
} lb_map_t;

typedef struct lb_map_iter_t {
  struct lb_map_record_t **table;
  struct lb_map_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  uint8_t first_step;
} lb_map_iter_t;

typedef struct lb_vector_t {
  uint8_t *buffer;
  uint8_t *swap_buffer;
  uint64_t value_size;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  uint64_t buffer_count;
} lb_vector_t;

#endif // LB_CONTAINER_FWD_H
