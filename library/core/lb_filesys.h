#ifndef LB_CORE_FILESYS_H
#define LB_CORE_FILESYS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void lb_filesys_load_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);
void lb_filesys_load_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path);

void lb_filesys_save_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path);
void lb_filesys_save_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_CORE_FILESYS_H
