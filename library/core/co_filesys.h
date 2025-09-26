#ifndef CO_FILESYS_H
#define CO_FILESYS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void filesys_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
void filesys_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

void filesys_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
void filesys_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CO_FILESYS_H
