#ifndef LB_CONTAINER_SET_H
#define LB_CONTAINER_SET_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void *lb_set_record_key(lb_set_record_t *record);
uint64_t lb_set_record_key_size(lb_set_record_t *record);
lb_set_record_t *lb_set_record_next(lb_set_record_t *record);

lb_set_t lb_set_create(void);
lb_set_t lb_set_copy(lb_set_t *reference);
uint8_t lb_set_equal(lb_set_t *set, lb_set_t *reference);
uint8_t lb_set_insert(lb_set_t *set, void const *key, uint64_t key_size);
uint8_t lb_set_remove(lb_set_t *set, void const *key, uint64_t key_size);
uint8_t lb_set_contains(lb_set_t *set, void const *key, uint64_t key_size);
uint64_t lb_set_table_size(lb_set_t *set);
uint64_t lb_set_table_count(lb_set_t *set);
lb_set_record_t *lb_set_table_at(lb_set_t *set, uint64_t index);
uint64_t lb_set_count(lb_set_t *set);
void lb_set_expand(lb_set_t *set);
void lb_set_clear(lb_set_t *set);
uint64_t lb_set_hash(lb_set_t *set, void const *key, uint64_t key_size, uint64_t modulus);
uint8_t lb_set_load_factor(lb_set_t *set);
void lb_set_destroy(lb_set_t *set);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_SET_H
