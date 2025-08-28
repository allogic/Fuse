#ifndef CORE_FILESYSTEM_H
#define CORE_FILESYSTEM_H

#include <stdint.h>

#include <library/core/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint8_t filesystem_read_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
uint8_t filesystem_read_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

uint8_t filesystem_write_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
uint8_t filesystem_write_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE_FILESYSTEM_H
