#ifndef LB_CONTAINER_MAP_H
#define LB_CONTAINER_MAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

lb_map_t lb_map_create(void);
lb_map_t lb_map_copy(lb_map_t *reference);
uint8_t lb_map_equal(lb_map_t *map, lb_map_t *reference);
uint8_t lb_map_insert(lb_map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size);
uint8_t lb_map_remove(lb_map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size);
uint8_t lb_map_contains(lb_map_t *map, void const *key, uint64_t key_size);
uint64_t lb_map_count(lb_map_t *map);
void *lb_map_at(lb_map_t *map, void const *key, uint64_t key_size);
void lb_map_expand(lb_map_t *map);
void lb_map_clear(lb_map_t *map);
lb_map_iter_t lb_map_iter(lb_map_t *map);
uint64_t lb_map_hash(lb_map_t *map, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t lb_map_load_factor(lb_map_t *map);
void lb_map_destroy(lb_map_t *map);

void *lb_map_record_key(lb_map_record_t *record);
uint64_t lb_map_record_key_size(lb_map_record_t *record);
void *lb_map_record_value(lb_map_record_t *record);
uint64_t lb_map_record_value_size(lb_map_record_t *record);

uint8_t lb_map_iter_step(lb_map_iter_t *iter);
void *lb_map_iter_key(lb_map_iter_t *iter);
uint64_t lb_map_iter_key_size(lb_map_iter_t *iter);
void *lb_map_iter_value(lb_map_iter_t *iter);
uint64_t lb_map_iter_value_size(lb_map_iter_t *iter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_MAP_H
