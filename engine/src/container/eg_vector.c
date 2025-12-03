#include <engine/eg_pch.h>

#include <engine/container/eg_vector.h>

#define EG_VECTOR_BUFFER_CAPACITY (16ULL)
#define EG_VECTOR_BUFFER_ALIGNMENT (16ULL)

struct eg_vector_t {
  uint8_t *buffer;
  uint8_t *swap_buffer;
  uint64_t value_size;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  uint64_t buffer_count;
};

eg_vector_t *eg_vector_create(uint64_t value_size) {
  eg_vector_t *vector = (eg_vector_t *)eg_heap_alloc(sizeof(eg_vector_t), 1, 0);

  vector->buffer = (uint8_t *)eg_heap_alloc(EG_ALIGN_UP_BY(EG_VECTOR_BUFFER_CAPACITY, EG_VECTOR_BUFFER_ALIGNMENT) * value_size, 1, 0);
  vector->swap_buffer = (uint8_t *)eg_heap_alloc(value_size, 1, 0);
  vector->value_size = value_size;
  vector->buffer_capacity = EG_ALIGN_UP_BY(EG_VECTOR_BUFFER_CAPACITY, EG_VECTOR_BUFFER_ALIGNMENT) * value_size;
  vector->buffer_size = 0;
  vector->buffer_count = 0;

  return vector;
}
eg_vector_t *eg_vector_create_from(uint64_t value_size, void const *buffer, uint64_t count) {
  eg_vector_t *vector = (eg_vector_t *)eg_heap_alloc(sizeof(eg_vector_t), 1, 0);

  vector->buffer = (uint8_t *)eg_heap_alloc(value_size * count, 0, buffer);
  vector->swap_buffer = (uint8_t *)eg_heap_alloc(value_size, 1, 0);
  vector->value_size = value_size;
  vector->buffer_capacity = EG_ALIGN_UP_BY(count, EG_VECTOR_BUFFER_ALIGNMENT) * value_size;
  vector->buffer_size = value_size * count;
  vector->buffer_count = count;

  return vector;
}
eg_vector_t *eg_vector_copy(eg_vector_t *reference) {
  eg_vector_t *vector = (eg_vector_t *)eg_heap_alloc(sizeof(eg_vector_t), 1, 0);

  vector->buffer = (uint8_t *)eg_heap_alloc(reference->buffer_size, 0, reference->buffer);
  vector->swap_buffer = (uint8_t *)eg_heap_alloc(reference->value_size, 1, 0);
  vector->value_size = reference->value_size;
  vector->buffer_capacity = reference->buffer_capacity;
  vector->buffer_size = reference->buffer_size;
  vector->buffer_count = reference->buffer_count;

  return vector;
}
uint8_t eg_vector_equal(eg_vector_t *vector, eg_vector_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= vector->value_size != reference->value_size;
  not_equal |= vector->buffer_size != reference->buffer_size;
  not_equal |= vector->buffer_count != reference->buffer_count;
  not_equal |= memcmp(vector->buffer, reference->buffer, EG_MIN(vector->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void eg_vector_fill(eg_vector_t *vector, void const *value) {
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
void eg_vector_push(eg_vector_t *vector, void const *value) {
  if (value) {
    memcpy(vector->buffer + vector->buffer_size, value, vector->value_size);
  }

  vector->buffer_count++;
  vector->buffer_size += vector->value_size;

  if (vector->buffer_size >= vector->buffer_capacity) {
    eg_vector_expand(vector);
  }
}
void eg_vector_push_range(eg_vector_t *vector, eg_vector_t *range) {
  if (range) {
    vector->buffer_count += range->buffer_count;
    vector->buffer_size += range->buffer_size;

    while (vector->buffer_size >= vector->buffer_capacity) {
      eg_vector_expand(vector);
    }

    memcpy(vector->buffer + vector->buffer_size - range->buffer_size, range->buffer, range->buffer_size);
  }
}
void eg_vector_pop(eg_vector_t *vector, void *value) {
  vector->buffer_count--;
  vector->buffer_size -= vector->value_size;

  if (value) {
    memcpy(value, vector->buffer + vector->buffer_size, vector->value_size);
  }
}
void eg_vector_resize(eg_vector_t *vector, uint64_t count) {
  uint64_t buffer_count = count;
  uint64_t buffer_size = count * vector->value_size;
  uint64_t buffer_capacity = 0;

  if (count) {
    buffer_capacity = EG_ALIGN_UP_BY(count, EG_VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;
  } else {
    buffer_capacity = EG_VECTOR_BUFFER_ALIGNMENT * vector->value_size * 2;
  }

  vector->buffer = (uint8_t *)eg_heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
  vector->buffer_size = buffer_size;
  vector->buffer_count = buffer_count;
}
void eg_vector_expand(eg_vector_t *vector) {
  uint64_t buffer_capacity = EG_ALIGN_UP_BY(vector->buffer_count, EG_VECTOR_BUFFER_ALIGNMENT) * vector->value_size * 2;

  vector->buffer = (uint8_t *)eg_heap_realloc(vector->buffer, buffer_capacity);
  vector->buffer_capacity = buffer_capacity;
}
void eg_vector_remove(eg_vector_t *vector, void const *value) {
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

    eg_vector_swap(vector, buffer_index, vector->buffer_count - 1);

    vector->buffer_count--;
    vector->buffer_size -= vector->value_size;
  }
}
void eg_vector_swap(eg_vector_t *vector, uint64_t left_index, uint64_t right_index) {
  uint8_t *left = vector->buffer + left_index * vector->value_size;
  uint8_t *right = vector->buffer + right_index * vector->value_size;

  memcpy(vector->swap_buffer, left, vector->value_size);
  memcpy(left, right, vector->value_size);
  memcpy(right, vector->swap_buffer, vector->value_size);
}
void eg_vector_clear(eg_vector_t *vector) {
  vector->buffer_size = 0;
  vector->buffer_count = 0;
}
void *eg_vector_back(eg_vector_t *vector) {
  return vector->buffer + vector->buffer_size - vector->value_size;
}
void *eg_vector_front(eg_vector_t *vector) {
  return vector->buffer;
}
void *eg_vector_at(eg_vector_t *vector, uint64_t index) {
  return vector->buffer + index * vector->value_size;
}
void *eg_vector_buffer(eg_vector_t *vector) {
  return vector->buffer;
}
void eg_vector_set(eg_vector_t *vector, uint64_t index, void const *value) {
  if (value) {
    memcpy(vector->buffer + index * vector->value_size, value, vector->value_size);
  }
}
uint8_t eg_vector_empty(eg_vector_t *vector) {
  return vector->buffer_count == 0;
}
uint64_t eg_vector_size(eg_vector_t *vector) {
  return vector->buffer_size;
}
uint64_t eg_vector_count(eg_vector_t *vector) {
  return vector->buffer_count;
}
uint64_t eg_vector_capacity(eg_vector_t *vector) {
  return vector->buffer_capacity;
}
void eg_vector_destroy(eg_vector_t *vector) {
  eg_heap_free(vector->buffer);
  eg_heap_free(vector->swap_buffer);
}
