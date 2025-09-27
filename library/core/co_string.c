#include <library/lb_pch.h>

#include <library/core/co_config.h>
#include <library/core/co_macros.h>
#include <library/core/co_heap.h>
#include <library/core/co_string.h>
#include <library/core/co_filesys.h>

string_t string_create(void) {
  string_t string = {0};

  string.buffer = (char *)heap_alloc(ALIGN_UP_BY(STRING_BUFFER_CAPACITY, STRING_BUFFER_ALIGNMENT));
  string.buffer_capacity = ALIGN_UP_BY(STRING_BUFFER_CAPACITY, STRING_BUFFER_ALIGNMENT);
  string.buffer_size = 0;
  string.buffer[0] = 0;

  return string;
}
string_t string_create_from(char const *value) {
  string_t string = string_create();

  uint64_t value_size = strlen(value);

  string_resize(&string, value_size);

  memcpy(string.buffer, value, value_size);

  string.buffer[string.buffer_size] = 0;

  return string;
}
string_t string_create_from_file(char const *input_file) {
  string_t string = string_create();

  uint8_t *buffer = 0;
  uint64_t buffer_size = 0;

  filesys_load_text(&buffer, &buffer_size, input_file);

  string_resize(&string, buffer_size);

  memcpy(string.buffer, buffer, buffer_size);

  string.buffer[string.buffer_size] = 0;

  return string;
}
void string_to_file(string_t *string, char const *output_file) {
  filesys_save_text(string->buffer, string->buffer_size, output_file);
}
string_t string_copy(string_t *reference) {
  string_t string = {0};

  string.buffer = (char *)heap_alloc(reference->buffer_capacity);
  string.buffer_capacity = reference->buffer_capacity;
  string.buffer_size = reference->buffer_size;
  memcpy(string.buffer, reference->buffer, reference->buffer_size);

  return string;
}
uint8_t string_equal(string_t *string, string_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= string->buffer_size != reference->buffer_size;
  not_equal |= memcmp(string->buffer, reference->buffer, MIN(string->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void string_fill(string_t *string, char const *value) {
  if (value) {
    memcpy(string->buffer, value, string->buffer_size);

    string->buffer[string->buffer_size] = 0;
  }
}
void string_upper(string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'a' && *ptr <= 'z') {
      *ptr -= 32;
    }

    ptr++;
  }
}
void string_lower(string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'A' && *ptr <= 'Z') {
      *ptr += 32;
    }

    ptr++;
  }
}
void string_append(string_t *string, char const *value) {
  uint64_t value_length = strlen(value);

  if (value) {
    string->buffer_size += value_length;

    while (string->buffer_size >= string->buffer_capacity) {
      string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

    string->buffer[string->buffer_size] = 0;
  }
}
void string_appends(string_t *string, char const *value, uint64_t size) {
  if (value) {
    string->buffer_size += size;

    while (string->buffer_size >= string->buffer_capacity) {
      string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - size, value, size);

    string->buffer[string->buffer_size] = 0;
  }
}
void string_appendv(string_t *string, uint64_t arg_count, ...) {
  uint64_t arg_index = 0;

  va_list args;
  va_start(args, arg_count);
  while (arg_index < arg_count) {
    char const *value = va_arg(args, char const *);

    uint64_t value_length = strlen(value);

    if (value) {
      string->buffer_size += value_length;

      while (string->buffer_size >= string->buffer_capacity) {
        string_expand(string);
      }

      memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

      string->buffer[string->buffer_size] = 0;
    }

    arg_index++;
  }
  va_end(args);
}
void string_appendf(string_t *string, char const *format, ...) {
  va_list args;

  va_start(args, format);
  uint64_t value_length = (uint64_t)vsnprintf(0, 0, format, args);
  va_end(args);

  while ((string->buffer_size + value_length) >= string->buffer_capacity) {
    string_expand(string);
  }

  va_start(args, format);
  vsnprintf(string->buffer + string->buffer_size, value_length + 1, format, args);
  va_end(args);

  string->buffer_size += value_length;
  string->buffer[string->buffer_size] = 0;
}
void string_resize(string_t *string, uint64_t size) {
  uint64_t buffer_size = size;
  uint64_t buffer_capacity = 0;

  if (size) {
    buffer_capacity = ALIGN_UP_BY(size + 1, STRING_BUFFER_ALIGNMENT);
  } else {
    buffer_capacity = ALIGN_UP_BY(STRING_BUFFER_CAPACITY, STRING_BUFFER_ALIGNMENT);
  }

  string->buffer = (char *)heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
  string->buffer_size = buffer_size;
  string->buffer[string->buffer_size] = 0;
}
void string_expand(string_t *string) {
  uint64_t buffer_capacity = ALIGN_UP_BY(string->buffer_capacity * 2, STRING_BUFFER_ALIGNMENT);

  string->buffer = (char *)heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
}
void string_clear(string_t *string) {
  string->buffer_size = 0;
  string->buffer[string->buffer_size] = 0;
}
char string_at(string_t *string, uint64_t index) {
  return string->buffer[index];
}
char *string_buffer(string_t *string) {
  return string->buffer;
}
void string_set(string_t *string, char const *value) {
  if (value) {
    uint64_t value_size = strlen(value);

    string->buffer_size = value_size;

    while (string->buffer_size >= string->buffer_capacity) {
      string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_size, value, value_size);

    string->buffer[string->buffer_size] = 0;
  }
}
uint8_t string_empty(string_t *string) {
  return string->buffer_size == 0;
}
uint64_t string_size(string_t *string) {
  return string->buffer_size;
}
uint64_t string_capacity(string_t *string) {
  return string->buffer_capacity;
}
void string_destroy(string_t *string) {
  heap_free(string->buffer);
}
