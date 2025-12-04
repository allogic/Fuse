#include <engine/eg_pch.h>

#include <engine/container/eg_string.h>

#define EG_STRING_BUFFER_CAPACITY (16ULL)
#define EG_STRING_BUFFER_ALIGNMENT (16ULL)

struct eg_string_t {
  char *buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
};

eg_string_t *eg_string_create(void) {
  eg_string_t *string = (eg_string_t *)eg_heap_alloc(sizeof(eg_string_t), 1, 0);

  string->buffer = (char *)eg_heap_alloc(EG_ALIGN_UP_BY(EG_STRING_BUFFER_CAPACITY, EG_STRING_BUFFER_ALIGNMENT), 1, 0);
  string->buffer_capacity = EG_ALIGN_UP_BY(EG_STRING_BUFFER_CAPACITY, EG_STRING_BUFFER_ALIGNMENT);
  string->buffer_size = 0;
  string->buffer[0] = 0;

  return string;
}
eg_string_t *eg_string_format(char const *format, ...) {
  eg_string_t *string = eg_string_create();

  va_list args;

  va_start(args, format);
  uint64_t value_length = (uint64_t)vsnprintf(0, 0, format, args);
  va_end(args);

  while ((string->buffer_size + value_length) >= string->buffer_capacity) {
    eg_string_expand(string);
  }

  va_start(args, format);
  vsnprintf(string->buffer + string->buffer_size, value_length + 1, format, args);
  va_end(args);

  string->buffer_size += value_length;
  string->buffer[string->buffer_size] = 0;

  return string;
}
eg_string_t *eg_string_value(char const *value) {
  eg_string_t *string = eg_string_create();

  if (value) {
    uint64_t value_size = strlen(value);

    eg_string_resize(string, value_size);

    memcpy(string->buffer, value, value_size);
  }

  string->buffer[string->buffer_size] = 0;

  return string;
}
eg_string_t *eg_string_values(char const *value, uint64_t value_size) {
  eg_string_t *string = eg_string_create();

  if (value) {
    eg_string_resize(string, value_size);

    memcpy(string->buffer, value, value_size);
  }

  string->buffer[string->buffer_size] = 0;

  return string;
}
eg_string_t *eg_string_file(char const *input_file) {
  eg_string_t *string = eg_string_create();

  uint8_t *buffer = 0;
  uint64_t buffer_size = 0;

  eg_filesys_load_text(&buffer, &buffer_size, input_file);

  if (buffer_size) {
    eg_string_resize(string, buffer_size);

    memcpy(string->buffer, buffer, buffer_size);
  }

  string->buffer[string->buffer_size] = 0;

  eg_heap_free(buffer);

  return string;
}
eg_string_t *eg_string_copy(eg_string_t *reference) {
  eg_string_t *string = (eg_string_t *)eg_heap_alloc(sizeof(eg_string_t), 1, 0);

  string->buffer = (char *)eg_heap_alloc(reference->buffer_capacity, 0, reference->buffer);
  string->buffer_capacity = reference->buffer_capacity;
  string->buffer_size = reference->buffer_size;

  return string;
}
eg_string_t *eg_string_substr(eg_string_t *string, uint64_t from, uint64_t to) {
  return eg_string_values(string->buffer, to - from);
}

int64_t eg_string_find(eg_string_t *string, char const *pattern) {
  int64_t found_pos = -1;

  uint64_t pattern_size = strlen(pattern);

  uint64_t buffer_pos = 0;
  uint64_t buffer_end = string->buffer_size;

  while (buffer_pos < buffer_end) {

    uint8_t equal = 1;

    uint64_t pattern_pos = 0;
    uint64_t pattern_end = pattern_size;

    if ((buffer_pos + pattern_size) >= buffer_end) {
      break;
    }

    while (pattern_pos < pattern_end) {

      char left = *(string->buffer + buffer_pos + pattern_pos);
      char right = *(pattern + pattern_pos);

      if (left != right) {
        equal = 0;
      }

      if (equal == 0) {
        break;
      }

      pattern_pos++;
    }

    if (equal) {

      found_pos = buffer_end - buffer_pos;

      break;
    }

    buffer_pos++;
  }

  return found_pos;
}
void eg_string_write(eg_string_t *string, char const *output_file) {
  eg_filesys_save_text(string->buffer, string->buffer_size, output_file);
}
uint8_t eg_string_equal(eg_string_t *string, eg_string_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= string->buffer_size != reference->buffer_size;
  not_equal |= memcmp(string->buffer, reference->buffer, EG_MIN(string->buffer_size, reference->buffer_size));

  return not_equal == 0;
}
void eg_string_fill(eg_string_t *string, char const *value) {
  if (value) {
    memcpy(string->buffer, value, string->buffer_size);

    string->buffer[string->buffer_size] = 0;
  }
}
void eg_string_upper(eg_string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'a' && *ptr <= 'z') {
      *ptr -= 32;
    }

    ptr++;
  }
}
void eg_string_lower(eg_string_t *string) {
  char *ptr = string->buffer;

  while (*ptr) {
    if (*ptr >= 'A' && *ptr <= 'Z') {
      *ptr += 32;
    }

    ptr++;
  }
}
void eg_string_append(eg_string_t *string, char const *value) {
  uint64_t value_length = strlen(value);

  if (value) {
    string->buffer_size += value_length;

    while (string->buffer_size >= string->buffer_capacity) {
      eg_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

    string->buffer[string->buffer_size] = 0;
  }
}
void eg_string_appends(eg_string_t *string, char const *value, uint64_t size) {
  if (value) {
    string->buffer_size += size;

    while (string->buffer_size >= string->buffer_capacity) {
      eg_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - size, value, size);

    string->buffer[string->buffer_size] = 0;
  }
}
void eg_string_appendv(eg_string_t *string, uint64_t arg_count, ...) {
  uint64_t arg_index = 0;

  va_list args;
  va_start(args, arg_count);
  while (arg_index < arg_count) {

    char const *value = va_arg(args, char const *);

    uint64_t value_length = strlen(value);

    if (value) {
      string->buffer_size += value_length;

      while (string->buffer_size >= string->buffer_capacity) {
        eg_string_expand(string);
      }

      memcpy(string->buffer + string->buffer_size - value_length, value, value_length);

      string->buffer[string->buffer_size] = 0;
    }

    arg_index++;
  }
  va_end(args);
}
void eg_string_appendf(eg_string_t *string, char const *format, ...) {
  va_list args;

  va_start(args, format);
  uint64_t value_length = (uint64_t)vsnprintf(0, 0, format, args);
  va_end(args);

  while ((string->buffer_size + value_length) >= string->buffer_capacity) {
    eg_string_expand(string);
  }

  va_start(args, format);
  vsnprintf(string->buffer + string->buffer_size, value_length + 1, format, args);
  va_end(args);

  string->buffer_size += value_length;
  string->buffer[string->buffer_size] = 0;
}
void eg_string_resize(eg_string_t *string, uint64_t size) {
  uint64_t buffer_size = size;
  uint64_t buffer_capacity = 0;

  if (size) {
    buffer_capacity = EG_ALIGN_UP_BY(size + 1, EG_STRING_BUFFER_ALIGNMENT);
  } else {
    buffer_capacity = EG_ALIGN_UP_BY(EG_STRING_BUFFER_CAPACITY, EG_STRING_BUFFER_ALIGNMENT);
  }

  string->buffer = (char *)eg_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
  string->buffer_size = buffer_size;
  string->buffer[string->buffer_size] = 0;
}
void eg_string_expand(eg_string_t *string) {
  uint64_t buffer_capacity = EG_ALIGN_UP_BY(string->buffer_capacity * 2, EG_STRING_BUFFER_ALIGNMENT);

  string->buffer = (char *)eg_heap_realloc(string->buffer, buffer_capacity);
  string->buffer_capacity = buffer_capacity;
}
void eg_string_clear(eg_string_t *string) {
  string->buffer_size = 0;
  string->buffer[string->buffer_size] = 0;
}
char eg_string_at(eg_string_t *string, uint64_t index) {
  return string->buffer[index];
}
char *eg_string_buffer(eg_string_t *string) {
  return string->buffer;
}
void eg_string_set(eg_string_t *string, char const *value) {
  if (value) {
    uint64_t value_size = strlen(value);

    string->buffer_size = value_size;

    while (string->buffer_size >= string->buffer_capacity) {
      eg_string_expand(string);
    }

    memcpy(string->buffer + string->buffer_size - value_size, value, value_size);

    string->buffer[string->buffer_size] = 0;
  }
}
uint8_t eg_string_empty(eg_string_t *string) {
  return string->buffer_size == 0;
}
uint64_t eg_string_size(eg_string_t *string) {
  return string->buffer_size;
}
uint64_t eg_string_capacity(eg_string_t *string) {
  return string->buffer_capacity;
}
void eg_string_destroy(eg_string_t *string) {
  eg_heap_free(string->buffer);
  eg_heap_free(string);
}
