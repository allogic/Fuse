#ifndef LB_CONTAINER_MAP_H
#define LB_CONTAINER_MAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

map_t map_create(void);
map_t map_copy(map_t *reference);
uint8_t map_equal(map_t *map, map_t *reference);
uint8_t map_insert(map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size);
uint8_t map_remove(map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size);
uint8_t map_contains(map_t *map, void const *key, uint64_t key_size);
uint64_t map_count(map_t *map);
void *map_at(map_t *map, void const *key, uint64_t key_size);
void map_expand(map_t *map);
void map_clear(map_t *map);
map_iter_t map_iter(map_t *map);
uint64_t map_hash(map_t *map, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t map_load_factor(map_t *map);
void map_destroy(map_t *map);

void *map_record_key(map_record_t *record);
uint64_t map_record_key_size(map_record_t *record);
void *map_record_value(map_record_t *record);
uint64_t map_record_value_size(map_record_t *record);

uint8_t map_iter_step(map_iter_t *iter);
void *map_iter_key(map_iter_t *iter);
uint64_t map_iter_key_size(map_iter_t *iter);
void *map_iter_value(map_iter_t *iter);
uint64_t map_iter_value_size(map_iter_t *iter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_MAP_H
