#include <string.h>

#include <library/core/config.h>
#include <library/core/macros.h>
#include <library/core/heap.h>
#include <library/core/vector.h>

vector_t vector_alloc(uint64_t value_size) {
  vector_t vector = {0};

  vector.buffer = (uint8_t *)heap_alloc(ALIGN_UP_BY(VECTOR_BUFFER_CAPACITY, VECTOR_BUFFER_ALIGNMENT) * value_size);
  vector.swap_buffer = (uint8_t *)heap_alloc(value_size);
  vector.value_size = value_size;
  vector.buffer_capacity = ALIGN_UP_BY(VECTOR_BUFFER_CAPACITY, VECTOR_BUFFER_ALIGNMENT) * value_size;
  vector.buffer_size = 0;
  vector.buffer_count = 0;

  return vector;
}
vector_t vector_copy(vector_t *reference) {
  vector_t vector = {0};

  vector.buffer = (uint8_t *)heap_alloc(reference->buffer_capacity);
  vector.swap_buffer = (uint8_t *)heap_alloc(reference->value_size);
  vector.value_size = reference->value_size;
  vector.buffer_capacity = reference->buffer_capacity;
  vector.buffer_size = reference->buffer_size;
  vector.buffer_count = reference->buffer_count;
  memcpy(vector.buffer, reference->buffer, reference->buffer_size);

  return vector;
}
uint8_t vector_equal(vector_t *vector, vector_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= vector->value_size != reference->value_size;
  not_equal |= vector->buffer_size != reference->buffer_size;
  not_equal |= vector->buffer_count != reference->buffer_count;
  not_equal |= memcmp(vector->buffer, reference->buffer, MIN(vector->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void vector_fill(vector_t *vector, void const *value) {
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
void vector_push(vector_t *vector, void const *value) {
  if (value) {
    memcpy(vector->buffer + vector->buffer_size, value, vector->value_size);
  }

  vector->buffer_count++;
  vector->buffer_size += vector->value_size;

  if (vector->buffer_size >= vector->buffer_capacity) {
    vector_expand(vector);
  }
}
void vector_push_range(vector_t *vector, vector_t *range) {
  if (range) {
    vector->buffer_count += range->buffer_count;
    vector->buffer_size += range->buffer_size;

    while (vector->buffer_size >= vector->buffer_capacity) {
      vector_expand(vector);
    }

    memcpy(vector->buffer + vector->buffer_size - range->buffer_size, range->buffer, range->buffer_size);
  }
}
void vector_pop(vector_t *vector, void *value) {
  vector->buffer_count--;
  vector->buffer_size -= vector->value_size;

  if (value) {
    memcpy(value, vector->buffer + vector->buffer_size, vector->value_size);
  }
}
void vector_resize(vector_t *vector, uint64_t count) {
  uint64_t buffer_count = count;
  uint64_t buffer_size = count * vector->value_size;
  uint64_t buffer_capacity = 0;

  if (count) {
    buffer_capacity = ALIGN_UP_BY(count, VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;
  } else {
    buffer_capacity = VECTOR_BUFFER_ALIGNMENT * vector->value_size * 2;
  }

  vector->buffer = (uint8_t *)heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
  vector->buffer_size = buffer_size;
  vector->buffer_count = buffer_count;
}
void vector_expand(vector_t *vector) {
  uint64_t buffer_capacity = ALIGN_UP_BY(vector->buffer_count, VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;

  vector->buffer = (uint8_t *)heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
}
void vector_remove(vector_t *vector, void const *value) {
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

    vector_swap(vector, buffer_index, vector->buffer_count - 1);

    vector->buffer_count--;
    vector->buffer_size -= vector->value_size;
  }
}
void vector_swap(vector_t *vector, uint64_t left_index, uint64_t right_index) {
  uint8_t *left = vector->buffer + left_index * vector->value_size;
  uint8_t *right = vector->buffer + right_index * vector->value_size;

  memcpy(vector->swap_buffer, left, vector->value_size);
  memcpy(left, right, vector->value_size);
  memcpy(right, vector->swap_buffer, vector->value_size);
}
void vector_clear(vector_t *vector) {
  vector->buffer_size = 0;
  vector->buffer_count = 0;
}
void *vector_back(vector_t *vector) {
  return vector->buffer + vector->buffer_size - vector->value_size;
}
void *vector_front(vector_t *vector) {
  return vector->buffer;
}
void *vector_at(vector_t *vector, uint64_t index) {
  return vector->buffer + index * vector->value_size;
}
void *vector_buffer(vector_t *vector) {
  return vector->buffer;
}
void vector_set(vector_t *vector, uint64_t index, void const *value) {
  if (value) {
    memcpy(vector->buffer + index * vector->value_size, value, vector->value_size);
  }
}
uint8_t vector_empty(vector_t *vector) {
  return vector->buffer_count == 0;
}
uint64_t vector_size(vector_t *vector) {
  return vector->buffer_size;
}
uint64_t vector_count(vector_t *vector) {
  return vector->buffer_count;
}
uint64_t vector_capacity(vector_t *vector) {
  return vector->buffer_capacity;
}
void vector_free(vector_t *vector) {
  heap_free(vector->buffer);
  heap_free(vector->swap_buffer);
}
