#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <stdint.h>

#include "library/core/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

string_t string_alloc(void);
string_t string_from(char const *value, uint64_t size);
string_t string_copy(string_t *reference);
uint8_t string_equal(string_t *string, string_t *reference);
void string_fill(string_t *string, char value);
void string_append(string_t *string, char const *value, uint64_t size);
void string_resize(string_t *string, uint64_t size);
void string_expand(string_t *string);
void string_clear(string_t *string);
char string_at(string_t *string, uint64_t index);
char *string_buffer(string_t *string);
void string_set(string_t *string, char const *value);
uint8_t string_empty(string_t *string);
uint64_t string_size(string_t *string);
uint64_t string_capacity(string_t *string);
void string_free(string_t *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_STRING_H
