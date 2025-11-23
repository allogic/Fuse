#include <container/lb_pch.h>
#include <container/lb_config.h>
#include <container/lb_macros.h>
#include <container/lb_string.h>

struct lb_string_t {
  char *buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
};

lb_string_t *lb_string_create(void) {
  lb_string_t *string = (lb_string_t *)lb_heap_alloc(sizeof(lb_string_t), 1, 0);

  string->buffer = (char *)lb_heap_alloc(LB_ALIGN_UP_BY(LB_STRING_BUFFER_CAPACITY, LB_STRING_BUFFER_ALIGNMENT), 1, 0);
  string->buffer_capacity = LB_ALIGN_UP_BY(LB_STRING_BUFFER_CAPACITY, LB_STRING_BUFFER_ALIGNMENT);
  string->buffer_size = 0;
  string->buffer[0] = 0;

  return string;
}
lb_string_t *lb_string_format(char const *format, ...) {
  lb_string_t *string = lb_string_create();

  va_list args;

  va_start(args, format);
  uint64_t value_length = (uint64_t)vsnprintf(0, 0, format, args);
  va_end(args);

  while ((string->buffer_size + value_length) >= string->buffer_capacity) {
    lb_string_expand(string);
  }

  va_start(args, format);
  vsnprintf(string->buffer + string->buffer_size, value_length + 1, format, args);
  va_end(args);

  string->buffer_size += value_length;
  string->buffer[string->buffer_size] = 0;

  return string;
}
lb_string_t *lb_string_value(char const *value) {
  lb_string_t *string = lb_string_create();

  if (value) {
    uint64_t value_size = strlen(value);

    lb_string_resize(string, value_size);

    memcpy(string->buffer, value, value_size);
  }

  string->buffer[string->buffer_size] = 0;

  return string;
}
lb_string_t *lb_string_create_file(char const *input_file) {
  lb_string_t *string = lb_string_create();

  uint8_t *buffer = 0;
  uint64_t buffer_size = 0;

  lb_filesys_load_text(&buffer, &buffer_size, input_file);

  if (buffer_size) {
    lb_string_resize(string, buffer_size);

    memcpy(string->buffer, buffer, buffer_size);
  }

  string->buffer[string->buffer_size] = 0;

  lb_heap_free(buffer);

  return string;
}

void lb_string_to_file(lb_string_t *string, char const *output_file) {
  lb_filesys_save_text(string->buffer, string->buffer_size, output_file);
}
lb_string_t *lb_string_copy(lb_string_t *reference) {
  lb_string_t *string = (lb_string_t *)lb_heap_alloc(sizeof(lb_string_t), 1, 0);

  string->buffer = (char *)lb_heap_alloc(reference->buffer_capacity, 0, reference->buffer);
  string->buffer_capacity = reference->buffer_capacity;
  string->buffer_size = reference->buffer_size;

  return string;
}
uint8_t lb_string_equal(lb_string_t *string, lb_string_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= string->buffer_size != reference->buffer_size;
  not_equal |= memcmp(string->buffer, reference->buffer, LB_MIN(string->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void lb_string_fill(lb_string_t *string, char const *value) {
  if (value) {
    memcpy(string->buffer, value, string->buffer_size);

    string->buffer[string->buffer_size] = 0;
  }
}
void lb_string_upper(lb_string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'a' && *ptr <= 'z') {
      *ptr -= 32;
    }

    ptr++;
  }
}
void lb_string_lower(lb_string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'A' && *ptr <= 'Z') {
      *ptr += 32;
    }

    ptr++;
  }
}
void lb_string_append(lb_string_t *string, char const *value) {
  uint64_t value_length = strlen(value);

  if (value) {
    string->buffer_size += value_length;

    while (string->buffer_size >= string->buffer_capacity) {
      lb_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

    string->buffer[string->buffer_size] = 0;
  }
}
void lb_string_appends(lb_string_t *string, char const *value, uint64_t size) {
  if (value) {
    string->buffer_size += size;

    while (string->buffer_size >= string->buffer_capacity) {
      lb_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - size, value, size);

    string->buffer[string->buffer_size] = 0;
  }
}
void lb_string_appendv(lb_string_t *string, uint64_t arg_count, ...) {
  uint64_t arg_index = 0;

  va_list args;
  va_start(args, arg_count);
  while (arg_index < arg_count) {

    char const *value = va_arg(args, char const *);

    uint64_t value_length = strlen(value);

    if (value) {
      string->buffer_size += value_length;

      while (string->buffer_size >= string->buffer_capacity) {
        lb_string_expand(string);
      }

      memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

      string->buffer[string->buffer_size] = 0;
    }

    arg_index++;
  }
  va_end(args);
}
void lb_string_appendf(lb_string_t *string, char const *format, ...) {
  va_list args;

  va_start(args, format);
  uint64_t value_length = (uint64_t)vsnprintf(0, 0, format, args);
  va_end(args);

  while ((string->buffer_size + value_length) >= string->buffer_capacity) {
    lb_string_expand(string);
  }

  va_start(args, format);
  vsnprintf(string->buffer + string->buffer_size, value_length + 1, format, args);
  va_end(args);

  string->buffer_size += value_length;
  string->buffer[string->buffer_size] = 0;
}
void lb_string_resize(lb_string_t *string, uint64_t size) {
  uint64_t buffer_size = size;
  uint64_t buffer_capacity = 0;

  if (size) {
    buffer_capacity = LB_ALIGN_UP_BY(size + 1, LB_STRING_BUFFER_ALIGNMENT);
  } else {
    buffer_capacity = LB_ALIGN_UP_BY(LB_STRING_BUFFER_CAPACITY, LB_STRING_BUFFER_ALIGNMENT);
  }

  string->buffer = (char *)lb_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
  string->buffer_size = buffer_size;
  string->buffer[string->buffer_size] = 0;
}
void lb_string_expand(lb_string_t *string) {
  uint64_t buffer_capacity = LB_ALIGN_UP_BY(string->buffer_capacity * 2, LB_STRING_BUFFER_ALIGNMENT);

  string->buffer = (char *)lb_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
}
void lb_string_clear(lb_string_t *string) {
  string->buffer_size = 0;
  string->buffer[string->buffer_size] = 0;
}
char lb_string_at(lb_string_t *string, uint64_t index) {
  return string->buffer[index];
}
char *lb_string_buffer(lb_string_t *string) {
  return string->buffer;
}
void lb_string_set(lb_string_t *string, char const *value) {
  if (value) {
    uint64_t value_size = strlen(value);

    string->buffer_size = value_size;

    while (string->buffer_size >= string->buffer_capacity) {
      lb_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_size, value, value_size);

    string->buffer[string->buffer_size] = 0;
  }
}
uint8_t lb_string_empty(lb_string_t *string) {
  return string->buffer_size == 0;
}
uint64_t lb_string_size(lb_string_t *string) {
  return string->buffer_size;
}
uint64_t lb_string_capacity(lb_string_t *string) {
  return string->buffer_capacity;
}
void lb_string_destroy(lb_string_t *string) {
  lb_heap_free(string->buffer);
}
