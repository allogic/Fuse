#ifndef EG_CONTAINER_MAP_H
#define EG_CONTAINER_MAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_map_t *eg_map_create(void);
eg_map_t *eg_map_copy(eg_map_t *reference);
uint8_t eg_map_equal(eg_map_t *map, eg_map_t *reference);
uint8_t eg_map_insert(eg_map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size);
uint8_t eg_map_remove(eg_map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size);
uint8_t eg_map_contains(eg_map_t *map, void const *key, uint64_t key_size);
uint64_t eg_map_count(eg_map_t *map);
void *eg_map_at(eg_map_t *map, void const *key, uint64_t key_size);
void eg_map_expand(eg_map_t *map);
void eg_map_clear(eg_map_t *map);
uint64_t eg_map_hash(eg_map_t *map, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t eg_map_load_factor(eg_map_t *map);
void eg_map_destroy(eg_map_t *map);

void *eg_map_record_key(eg_map_record_t *record);
uint64_t eg_map_record_key_size(eg_map_record_t *record);
void *eg_map_record_value(eg_map_record_t *record);
uint64_t eg_map_record_value_size(eg_map_record_t *record);

eg_map_iter_t *eg_map_iter_create(eg_map_t *map);
uint8_t eg_map_iter_step(eg_map_iter_t *iter);
void *eg_map_iter_key(eg_map_iter_t *iter);
uint64_t eg_map_iter_key_size(eg_map_iter_t *iter);
void *eg_map_iter_value(eg_map_iter_t *iter);
uint64_t eg_map_iter_value_size(eg_map_iter_t *iter);
void eg_map_iter_destroy(eg_map_iter_t *iter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTAINER_MAP_H
