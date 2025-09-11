#ifndef CORE_VECTOR_H
#define CORE_VECTOR_H

#include <stdint.h>

#include <library/core/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

vector_t vector_create(uint64_t value_size);
vector_t vector_copy(vector_t *reference);
uint8_t vector_equal(vector_t *vector, vector_t *reference);
void vector_fill(vector_t *vector, void const *value);
void vector_push(vector_t *vector, void const *value);
void vector_push_range(vector_t *vector, vector_t *range);
void vector_pop(vector_t *vector, void *value);
void vector_resize(vector_t *vector, uint64_t count);
void vector_expand(vector_t *vector);
void vector_remove(vector_t *vector, void const *value);
void vector_swap(vector_t *vector, uint64_t left_index, uint64_t right_index);
void vector_clear(vector_t *vector);
void *vector_back(vector_t *vector);
void *vector_front(vector_t *vector);
void *vector_at(vector_t *vector, uint64_t index);
void *vector_buffer(vector_t *vector);
void vector_set(vector_t *vector, uint64_t index, void const *value);
uint8_t vector_empty(vector_t *vector);
uint64_t vector_size(vector_t *vector);
uint64_t vector_count(vector_t *vector);
uint64_t vector_capacity(vector_t *vector);
void vector_free(vector_t *vector);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_VECTOR_H
