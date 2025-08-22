#ifndef CORE_VECTOR_H
#define CORE_VECTOR_H

#include <stdint.h>

#include "engine/core/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

core_vector_t core_vector_alloc(uint64_t value_size);
core_vector_t core_vector_copy(core_vector_t *reference);
uint8_t core_vector_equal(core_vector_t *vector, core_vector_t *reference);
void core_vector_fill(core_vector_t *vector, void const *value);
void core_vector_push(core_vector_t *vector, void const *value);
void core_vector_push_range(core_vector_t *vector, core_vector_t *range);
void core_vector_pop(core_vector_t *vector, void *value);
void core_vector_resize(core_vector_t *vector, uint64_t count);
void core_vector_expand(core_vector_t *vector);
void core_vector_remove(core_vector_t *vector, void const *value);
void core_vector_swap(core_vector_t *vector, uint64_t left_index, uint64_t right_index);
void core_vector_clear(core_vector_t *vector);
void *core_vector_back(core_vector_t *vector);
void *core_vector_front(core_vector_t *vector);
void *core_vector_at(core_vector_t *vector, uint64_t index);
void *core_vector_buffer(core_vector_t *vector);
void core_vector_set(core_vector_t *vector, uint64_t index, void const *value);
uint8_t core_vector_empty(core_vector_t *vector);
uint64_t core_vector_size(core_vector_t *vector);
uint64_t core_vector_count(core_vector_t *vector);
uint64_t core_vector_capacity(core_vector_t *vector);
void core_vector_free(core_vector_t *vector);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_VECTOR_H
