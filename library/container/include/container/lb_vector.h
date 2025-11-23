#ifndef LB_CONTAINER_VECTOR_H
#define LB_CONTAINER_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

lb_vector_t *lb_vector_create(uint64_t value_size);
lb_vector_t *lb_vector_create_from(uint64_t value_size, void const *buffer, uint64_t count);
lb_vector_t *lb_vector_copy(lb_vector_t *reference);
uint8_t lb_vector_equal(lb_vector_t *vector, lb_vector_t *reference);
void lb_vector_fill(lb_vector_t *vector, void const *value);
void lb_vector_push(lb_vector_t *vector, void const *value);
void lb_vector_push_range(lb_vector_t *vector, lb_vector_t *range);
void lb_vector_pop(lb_vector_t *vector, void *value);
void lb_vector_resize(lb_vector_t *vector, uint64_t count);
void lb_vector_expand(lb_vector_t *vector);
void lb_vector_remove(lb_vector_t *vector, void const *value);
void lb_vector_swap(lb_vector_t *vector, uint64_t left_index, uint64_t right_index);
void lb_vector_clear(lb_vector_t *vector);
void *lb_vector_back(lb_vector_t *vector);
void *lb_vector_front(lb_vector_t *vector);
void *lb_vector_at(lb_vector_t *vector, uint64_t index);
void *lb_vector_buffer(lb_vector_t *vector);
void lb_vector_set(lb_vector_t *vector, uint64_t index, void const *value);
uint8_t lb_vector_empty(lb_vector_t *vector);
uint64_t lb_vector_size(lb_vector_t *vector);
uint64_t lb_vector_count(lb_vector_t *vector);
uint64_t lb_vector_capacity(lb_vector_t *vector);
void lb_vector_destroy(lb_vector_t *vector);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_VECTOR_H
