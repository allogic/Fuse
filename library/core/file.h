#ifndef CORE_FILE_H
#define CORE_FILE_H

#include <stdint.h>

#include <library/core/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

file_t file_alloc(void);
file_t file_load_text(char const *file_path);
file_t file_load_binary(char const *file_path);
void file_save_binary(file_t *file, char const *file_path);
uint64_t file_size(file_t *file);
uint8_t *file_buffer(file_t *file);
void file_free(file_t *file);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORE__H
