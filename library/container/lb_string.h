#ifndef LB_CONTAINER_STRING_H
#define LB_CONTAINER_STRING_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

lb_string_t lb_string_create(void);
lb_string_t lb_string_format(char const *format, ...);
lb_string_t lb_string_create_from(char const *value);           // TODO: Deprecated..
lb_string_t lb_string_create_from_file(char const *input_file); // TODO: Deprecated..

void lb_string_to_file(lb_string_t *string, char const *output_file);
lb_string_t lb_string_copy(lb_string_t *reference);
uint8_t lb_string_equal(lb_string_t *string, lb_string_t *reference);
void lb_string_fill(lb_string_t *string, char const *value);
void lb_string_upper(lb_string_t *string);
void lb_string_lower(lb_string_t *string);
void lb_string_append(lb_string_t *string, char const *value);
void lb_string_appends(lb_string_t *string, char const *value, uint64_t size);
void lb_string_appendv(lb_string_t *string, uint64_t arg_count, ...);
void lb_string_appendf(lb_string_t *string, char const *format, ...);
void lb_string_resize(lb_string_t *string, uint64_t size);
void lb_string_expand(lb_string_t *string);
void lb_string_clear(lb_string_t *string);
char lb_string_at(lb_string_t *string, uint64_t index);
char *lb_string_buffer(lb_string_t *string);
void lb_string_set(lb_string_t *string, char const *value);
uint8_t lb_string_empty(lb_string_t *string);
uint64_t lb_string_size(lb_string_t *string);
uint64_t lb_string_capacity(lb_string_t *string);
void lb_string_destroy(lb_string_t *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CONTAINER_STRING_H
