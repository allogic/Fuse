#ifndef CO_SET_H
#define CO_SET_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void *set_entry_key(set_entry_t *entry);
uint64_t set_entry_key_size(set_entry_t *entry);
set_entry_t *set_entry_next(set_entry_t *entry);

set_t set_create(void);
set_t set_copy(set_t *reference);
uint8_t set_equal(set_t *set, set_t *reference);
uint8_t set_insert(set_t *set, void const *key, uint64_t key_size);
uint8_t set_remove(set_t *set, void const *key, uint64_t key_size);
uint8_t set_contains(set_t *set, void const *key, uint64_t key_size);
uint64_t set_table_size(set_t *set);
uint64_t set_table_count(set_t *set);
set_entry_t *set_table_at(set_t *set, uint64_t index);
uint64_t set_count(set_t *set);
void set_expand(set_t *set);
void set_clear(set_t *set);
uint64_t set_hash(set_t *set, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t set_load_factor(set_t *set);
void set_destroy(set_t *set);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CO_SET_H
