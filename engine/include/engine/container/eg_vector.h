#ifndef EG_CONTAINER_VECTOR_H
#define EG_CONTAINER_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_vector_t *eg_vector_create(uint64_t value_size);
eg_vector_t *eg_vector_create_from(uint64_t value_size, void const *buffer, uint64_t count);
eg_vector_t *eg_vector_copy(eg_vector_t *reference);
uint8_t eg_vector_equal(eg_vector_t *vector, eg_vector_t *reference);
void eg_vector_fill(eg_vector_t *vector, void const *value);
void eg_vector_push(eg_vector_t *vector, void const *value);
void eg_vector_push_range(eg_vector_t *vector, eg_vector_t *range);
void eg_vector_pop(eg_vector_t *vector, void *value);
void eg_vector_resize(eg_vector_t *vector, uint64_t count);
void eg_vector_expand(eg_vector_t *vector);
void eg_vector_remove(eg_vector_t *vector, void const *value);
void eg_vector_swap(eg_vector_t *vector, uint64_t left_index, uint64_t right_index);
void eg_vector_clear(eg_vector_t *vector);
void *eg_vector_back(eg_vector_t *vector);
void *eg_vector_front(eg_vector_t *vector);
void *eg_vector_at(eg_vector_t *vector, uint64_t index);
void *eg_vector_buffer(eg_vector_t *vector);
void eg_vector_set(eg_vector_t *vector, uint64_t index, void const *value);
uint8_t eg_vector_empty(eg_vector_t *vector);
uint64_t eg_vector_size(eg_vector_t *vector);
uint64_t eg_vector_count(eg_vector_t *vector);
uint64_t eg_vector_capacity(eg_vector_t *vector);
void eg_vector_destroy(eg_vector_t *vector);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTAINER_VECTOR_H
