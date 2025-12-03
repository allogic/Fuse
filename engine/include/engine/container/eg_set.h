#ifndef EG_CONTAINER_SET_H
#define EG_CONTAINER_SET_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_set_t *eg_set_create(void);
eg_set_t *eg_set_copy(eg_set_t *reference);
uint8_t eg_set_equal(eg_set_t *set, eg_set_t *reference);
uint8_t eg_set_insert(eg_set_t *set, void const *key, uint64_t key_size);
uint8_t eg_set_remove(eg_set_t *set, void const *key, uint64_t key_size);
uint8_t eg_set_contains(eg_set_t *set, void const *key, uint64_t key_size);
uint64_t eg_set_table_size(eg_set_t *set);
uint64_t eg_set_table_count(eg_set_t *set);
eg_set_record_t *eg_set_table_at(eg_set_t *set, uint64_t index);
uint64_t eg_set_count(eg_set_t *set);
void eg_set_expand(eg_set_t *set);
void eg_set_clear(eg_set_t *set);
uint64_t eg_set_hash(eg_set_t *set, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t eg_set_load_factor(eg_set_t *set);
void eg_set_destroy(eg_set_t *set);

void *eg_set_record_key(eg_set_record_t *record);
uint64_t eg_set_record_key_size(eg_set_record_t *record);

eg_set_iter_t *eg_set_iter_create(eg_set_t *set);
uint8_t eg_set_iter_step(eg_set_iter_t *iter);
void *eg_set_iter_key(eg_set_iter_t *iter);
uint64_t eg_set_iter_key_size(eg_set_iter_t *iter);
void eg_set_iter_destroy(eg_set_iter_t *iter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTAINER_SET_H
