#ifndef CORE_MAP_H
#define CORE_MAP_H

#include <stdint.h>

#include <library/core/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void *map_pair_key(map_pair_t *pair);
uint64_t map_pair_key_size(map_pair_t *pair);
void *map_pair_value(map_pair_t *pair);
uint64_t map_pair_value_size(map_pair_t *pair);
map_pair_t *map_pair_next(map_pair_t *pair);

map_t map_alloc(void);
map_t map_copy(map_t *reference);
uint8_t map_equal(map_t *map, map_t *reference);
uint8_t map_insert(map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size);
uint8_t map_remove(map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size);
uint8_t map_contains(map_t *map, void const *key, uint64_t key_size);
uint64_t map_table_size(map_t *map);
uint64_t map_table_count(map_t *map);
map_pair_t *map_table_at(map_t *map, uint64_t index);
uint64_t map_count(map_t *map);
void *map_at(map_t *map, void const *key, uint64_t key_size);
void map_expand(map_t *map);
void map_clear(map_t *map);
uint64_t map_hash(map_t *map, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t map_load_factor(map_t *map);
void map_free(map_t *map);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_MAP_H
