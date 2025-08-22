#include <stdio.h>
#include <stdlib.h>

#include <library/core/filesystem.h>
#include <library/core/heap.h>

uint8_t core_filesystem_read_text(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "r");

  if (file) {
    fseek(file, 0, SEEK_END);
    (*buffer_size) = (int unsigned)ftell(file);
    (*buffer) = (char *)core_heap_alloc(((*buffer_size) + 1));
    fseek(file, 0, SEEK_SET);
    fread((*buffer), sizeof(char), (*buffer_size), file);
    fclose(file);

    return 1;
  }

  return 0;
}
uint8_t core_filesystem_read_binary(uint8_t **buffer, uint64_t *buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    (*buffer_size) = (int unsigned)ftell(file);
    (*buffer) = (char unsigned *)core_heap_alloc(*buffer_size);
    fseek(file, 0, SEEK_SET);
    fread((*buffer), sizeof(char unsigned), (*buffer_size), file);
    fclose(file);

    return 1;
  }

  return 0;
}

uint8_t core_filesystem_write_text(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "w");

  if (file) {
    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(char), buffer_size, file);
    fclose(file);

    return 1;
  }

  return 0;
}
uint8_t core_filesystem_write_binary(uint8_t *buffer, uint64_t buffer_size, char const *file_path) {
  FILE *file = 0;

  fopen_s(&file, file_path, "wb");

  if (file) {
    fseek(file, 0, SEEK_SET);
    fwrite(buffer, sizeof(char unsigned), buffer_size, file);
    fclose(file);

    return 1;
  }

  return 0;
}
