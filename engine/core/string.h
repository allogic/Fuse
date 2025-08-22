#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <stdint.h>

#include "engine/core/forward.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

core_string_t core_string_alloc(void);
core_string_t core_string_from(char const *value, uint64_t size);
core_string_t core_string_copy(core_string_t *reference);
uint8_t core_string_equal(core_string_t *string, core_string_t *reference);
void core_string_fill(core_string_t *string, char value);
void core_string_append(core_string_t *string, char const *value, uint64_t size);
void core_string_resize(core_string_t *string, uint64_t size);
void core_string_expand(core_string_t *string);
void core_string_clear(core_string_t *string);
char core_string_at(core_string_t *string, uint64_t index);
char *core_string_buffer(core_string_t *string);
void core_string_set(core_string_t *string, char const *value);
uint8_t core_string_empty(core_string_t *string);
uint64_t core_string_size(core_string_t *string);
uint64_t core_string_capacity(core_string_t *string);
void core_string_free(core_string_t *string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_STRING_H
