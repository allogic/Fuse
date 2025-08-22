#include <stdio.h>
#include <string.h>

#include "engine/core/config.h"
#include "engine/core/macros.h"
#include "engine/core/heap.h"
#include "engine/core/vector.h"

core_vector_t core_vector_alloc(uint64_t value_size) {
  core_vector_t vector;
  memset(&vector, 0, sizeof(core_vector_t));

  vector.buffer = (uint8_t *)core_heap_alloc(CORE_ALIGN_UP_BY(CORE_VECTOR_BUFFER_CAPACITY, CORE_VECTOR_BUFFER_ALIGNMENT) * value_size);
  vector.swap_buffer = (uint8_t *)core_heap_alloc(value_size);
  vector.value_size = value_size;
  vector.buffer_capacity = CORE_ALIGN_UP_BY(CORE_VECTOR_BUFFER_CAPACITY, CORE_VECTOR_BUFFER_ALIGNMENT) * value_size;
  vector.buffer_size = 0;
  vector.buffer_count = 0;

  return vector;
}
core_vector_t core_vector_copy(core_vector_t *reference) {
  core_vector_t vector;
  memset(&vector, 0, sizeof(core_vector_t));

  vector.buffer = (uint8_t *)core_heap_alloc(reference->buffer_capacity);
  vector.swap_buffer = (uint8_t *)core_heap_alloc(reference->value_size);
  vector.value_size = reference->value_size;
  vector.buffer_capacity = reference->buffer_capacity;
  vector.buffer_size = reference->buffer_size;
  vector.buffer_count = reference->buffer_count;
  memcpy(vector.buffer, reference->buffer, reference->buffer_size);

  return vector;
}
uint8_t core_vector_equal(core_vector_t *vector, core_vector_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= vector->value_size != reference->value_size;
  not_equal |= vector->buffer_size != reference->buffer_size;
  not_equal |= vector->buffer_count != reference->buffer_count;
  not_equal |= memcmp(vector->buffer, reference->buffer, CORE_MIN(vector->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void core_vector_fill(core_vector_t *vector, void const *value) {
  if (value) {
    uint64_t buffer_index = 0;
    uint64_t buffer_offset = 0;

    while (buffer_index < vector->buffer_count) {
      memcpy(vector->buffer + buffer_offset, value, vector->buffer_size);

      buffer_index++;
      buffer_offset += vector->value_size;
    }
  }
}
void core_vector_push(core_vector_t *vector, void const *value) {
  if (value) {
    memcpy(vector->buffer + vector->buffer_size, value, vector->value_size);
  }

  vector->buffer_count++;
  vector->buffer_size += vector->value_size;

  if (vector->buffer_size >= vector->buffer_capacity) {
    core_vector_expand(vector);
  }
}
void core_vector_push_range(core_vector_t *vector, core_vector_t *range) {
  if (range) {
    vector->buffer_count += range->buffer_count;
    vector->buffer_size += range->buffer_size;

    while (vector->buffer_size >= vector->buffer_capacity) {
      core_vector_expand(vector);
    }

    memcpy(vector->buffer + vector->buffer_size - range->buffer_size, range->buffer, range->buffer_size);
  }
}
void core_vector_pop(core_vector_t *vector, void *value) {
  vector->buffer_count--;
  vector->buffer_size -= vector->value_size;

  if (value) {
    memcpy(value, vector->buffer + vector->buffer_size, vector->value_size);
  }
}
void core_vector_resize(core_vector_t *vector, uint64_t count) {
  uint64_t buffer_count = count;
  uint64_t buffer_size = count * vector->value_size;
  uint64_t buffer_capacity = 0;

  if (count) {
    buffer_capacity = CORE_ALIGN_UP_BY(count, CORE_VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;
  } else {
    buffer_capacity = CORE_VECTOR_BUFFER_ALIGNMENT * vector->value_size * 2;
  }

  vector->buffer = (uint8_t *)core_heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
  vector->buffer_size = buffer_size;
  vector->buffer_count = buffer_count;
}
void core_vector_expand(core_vector_t *vector) {
  uint64_t buffer_capacity = CORE_ALIGN_UP_BY(vector->buffer_count, CORE_VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;

  vector->buffer = (uint8_t *)core_heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
}
void core_vector_remove(core_vector_t *vector, void const *value) {
  if (value) {
    uint64_t buffer_index = 0;
    uint64_t buffer_offset = 0;

    while (buffer_index < vector->buffer_count) {
      if (memcmp(vector->buffer + buffer_offset, value, vector->value_size) == 0) {
        break;
      }

      buffer_index++;
      buffer_offset += vector->value_size;
    }

    core_vector_swap(vector, buffer_index, vector->buffer_count - 1);

    vector->buffer_count--;
    vector->buffer_size -= vector->value_size;
  }
}
void core_vector_swap(core_vector_t *vector, uint64_t left_index, uint64_t right_index) {
  uint8_t *left = vector->buffer + left_index * vector->value_size;
  uint8_t *right = vector->buffer + right_index * vector->value_size;

  memcpy(vector->swap_buffer, left, vector->value_size);
  memcpy(left, right, vector->value_size);
  memcpy(right, vector->swap_buffer, vector->value_size);
}
void core_vector_clear(core_vector_t *vector) {
  vector->buffer_size = 0;
  vector->buffer_count = 0;
}
void *core_vector_back(core_vector_t *vector) {
  return vector->buffer + vector->buffer_size - vector->value_size;
}
void *core_vector_front(core_vector_t *vector) {
  return vector->buffer;
}
void *core_vector_at(core_vector_t *vector, uint64_t index) {
  return vector->buffer + index * vector->value_size;
}
void *core_vector_buffer(core_vector_t *vector) {
  return vector->buffer;
}
void core_vector_set(core_vector_t *vector, uint64_t index, void const *value) {
  if (value) {
    memcpy(vector->buffer + index * vector->value_size, value, vector->value_size);
  }
}
uint8_t core_vector_empty(core_vector_t *vector) {
  return vector->buffer_count == 0;
}
uint64_t core_vector_size(core_vector_t *vector) {
  return vector->buffer_size;
}
uint64_t core_vector_count(core_vector_t *vector) {
  return vector->buffer_count;
}
uint64_t core_vector_capacity(core_vector_t *vector) {
  return vector->buffer_capacity;
}
void core_vector_free(core_vector_t *vector) {
  core_heap_free(vector->buffer);
  core_heap_free(vector->swap_buffer);

  memset(vector, 0, sizeof(core_vector_t));
}
