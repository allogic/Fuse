#include <stdio.h>
#include <string.h>

#include "library/core/config.h"
#include "library/core/macros.h"
#include "library/core/heap.h"
#include "library/core/string.h"

core_string_t core_string_alloc(void) {
  core_string_t string = {0};

  string.buffer = (char *)core_heap_alloc(CORE_ALIGN_UP_BY(CORE_STRING_BUFFER_CAPACITY, CORE_STRING_BUFFER_ALIGNMENT));
  string.buffer_capacity = CORE_ALIGN_UP_BY(CORE_STRING_BUFFER_CAPACITY, CORE_STRING_BUFFER_ALIGNMENT);
  string.buffer_size = 0;
  string.buffer[0] = 0;

  return string;
}
core_string_t core_string_copy(core_string_t *reference) {
  core_string_t string = {0};

  string.buffer = (char *)core_heap_alloc(reference->buffer_capacity);
  string.buffer_capacity = reference->buffer_capacity;
  string.buffer_size = reference->buffer_size;
  memcpy(string.buffer, reference->buffer, reference->buffer_size);

  return string;
}
core_string_t core_string_from(char const *value, uint64_t size) {
  core_string_t string = core_string_alloc();

  if (value) {
    core_string_append(&string, value, size);
  }

  return string;
}
uint8_t core_string_equal(core_string_t *string, core_string_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= string->buffer_size != reference->buffer_size;
  not_equal |= memcmp(string->buffer, reference->buffer, CORE_MIN(string->buffer_size, string->buffer_size));

  return not_equal == 0;
}
void core_string_fill(core_string_t *string, char value) {
  if (value) {
    memset(string->buffer, value, string->buffer_size);

    string->buffer[string->buffer_size] = 0;
  }
}
void core_string_append(core_string_t *string, char const *value, uint64_t size) {
  if (value) {
    string->buffer_size += size;

    while (string->buffer_size >= string->buffer_capacity) {
      core_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - size, value, size);

    string->buffer[string->buffer_size] = 0;
  }
}
void core_string_resize(core_string_t *string, uint64_t size) {
  uint64_t buffer_size = size;
  uint64_t buffer_capacity = 0;

  if (size) {
    buffer_capacity = CORE_ALIGN_UP_BY(size + 1, CORE_STRING_BUFFER_ALIGNMENT);
  } else {
    buffer_capacity = CORE_ALIGN_UP_BY(CORE_STRING_BUFFER_CAPACITY, CORE_STRING_BUFFER_ALIGNMENT);
  }

  string->buffer = (char *)core_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
  string->buffer_size = buffer_size;
  string->buffer[string->buffer_size] = 0;
}
void core_string_expand(core_string_t *string) {
  uint64_t buffer_capacity = CORE_ALIGN_UP_BY(string->buffer_size + 1, CORE_STRING_BUFFER_ALIGNMENT);

  string->buffer = (char *)core_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
}
void core_string_clear(core_string_t *string) {
  string->buffer_size = 0;
  string->buffer[string->buffer_size] = 0;
}
char core_string_at(core_string_t *string, uint64_t index) {
  return string->buffer[index];
}
char *core_string_buffer(core_string_t *string) {
  return string->buffer;
}
void core_string_set(core_string_t *string, char const *value) {
  if (value) {
    uint64_t value_size = strlen(value);

    string->buffer_size = value_size;

    while (string->buffer_size >= string->buffer_capacity) {
      core_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_size, value, value_size);

    string->buffer[string->buffer_size] = 0;
  }
}
uint8_t core_string_empty(core_string_t *string) {
  return string->buffer_size == 0;
}
uint64_t core_string_size(core_string_t *string) {
  return string->buffer_size;
}
uint64_t core_string_capacity(core_string_t *string) {
  return string->buffer_capacity;
}
void core_string_free(core_string_t *string) {
  core_heap_free(string->buffer);
}
