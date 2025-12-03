#ifndef EG_CONTAINER_STRING_H
#define EG_CONTAINER_STRING_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_string_t *eg_string_create(void);
eg_string_t *eg_string_format(char const *format, ...);
eg_string_t *eg_string_value(char const *value);
eg_string_t *eg_string_file(char const *input_file);

void eg_string_to_file(eg_string_t *string, char const *output_file);
eg_string_t *eg_string_copy(eg_string_t *reference);
uint8_t eg_string_equal(eg_string_t *string, eg_string_t *reference);
void eg_string_fill(eg_string_t *string, char const *value);
void eg_string_upper(eg_string_t *string);
void eg_string_lower(eg_string_t *string);
void eg_string_append(eg_string_t *string, char const *value);
void eg_string_appends(eg_string_t *string, char const *value, uint64_t size);
void eg_string_appendv(eg_string_t *string, uint64_t arg_count, ...);
void eg_string_appendf(eg_string_t *string, char const *format, ...);
void eg_string_resize(eg_string_t *string, uint64_t size);
void eg_string_expand(eg_string_t *string);
void eg_string_clear(eg_string_t *string);
char eg_string_at(eg_string_t *string, uint64_t index);
char *eg_string_buffer(eg_string_t *string);
void eg_string_set(eg_string_t *string, char const *value);
uint8_t eg_string_empty(eg_string_t *string);
uint64_t eg_string_size(eg_string_t *string);
uint64_t eg_string_capacity(eg_string_t *string);
void eg_string_destroy(eg_string_t *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTAINER_STRING_H
