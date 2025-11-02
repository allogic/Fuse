#ifndef LB_CONTAINER_STRING_H
#define LB_CONTAINER_STRING_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

string_t string_create(void);
string_t string_format(char const *format, ...);
string_t string_create_from(char const *value);           // TODO: Depreicated..
string_t string_create_from_file(char const *input_file); // TODO: Depreicated..

void string_to_file(string_t *string, char const *output_file);
string_t string_copy(string_t *reference);
uint8_t string_equal(string_t *string, string_t *reference);
void string_fill(string_t *string, char const *value);
void string_upper(string_t *string);
void string_lower(string_t *string);
void string_append(string_t *string, char const *value);
void string_appends(string_t *string, char const *value, uint64_t size);
void string_appendv(string_t *string, uint64_t arg_count, ...);
void string_appendf(string_t *string, char const *format, ...);
void string_resize(string_t *string, uint64_t size);
void string_expand(string_t *string);
void string_clear(string_t *string);
char string_at(string_t *string, uint64_t index);
char *string_buffer(string_t *string);
void string_set(string_t *string, char const *value);
uint8_t string_empty(string_t *string);
uint64_t string_size(string_t *string);
uint64_t string_capacity(string_t *string);
void string_destroy(string_t *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_STRING_H
