#include <stdio.h>
#include <stdlib.h>

#include <library/core/heap.h>

file_t file_alloc(void) {
  file_t file = {0};

  file.file_path = 0;
  file.buffer = 0;
  file.buffer_size = 0;

  return file;
}
file_t file_load_text(char const *file_path) {
  file_t file = {0};

  file.file_path = file_path;
  file.buffer = 0;
  file.buffer_size = 0;

  FILE *raw_file = 0;

  fopen_s(&raw_file, file_path, "r");

  if (raw_file) {
    fseek(raw_file, 0, SEEK_END);

    file.buffer_size = (uint64_t)ftell(raw_file);
    file.buffer = (uint8_t *)heap_alloc(file.buffer_size + 1);

    fseek(raw_file, 0, SEEK_SET);
    fread(file.buffer, sizeof(char), file.buffer_size, raw_file);

    fclose(raw_file);
  }

  return file;
}
file_t file_load_binary(char const *file_path) {
  file_t file = {0};

  file.file_path = file_path;
  file.buffer = 0;
  file.buffer_size = 0;

  FILE *raw_file = 0;

  fopen_s(&raw_file, file_path, "rb");

  if (raw_file) {
    fseek(raw_file, 0, SEEK_END);

    file.buffer_size = (uint64_t)ftell(raw_file);
    file.buffer = (uint8_t *)heap_alloc(file.buffer_size);

    fseek(raw_file, 0, SEEK_SET);
    fread(file.buffer, sizeof(uint8_t), file.buffer_size, raw_file);

    fclose(raw_file);
  }

  return file;
}
void file_save_text(file_t *file, char const *file_path) {
  FILE *raw_file = 0;

  fopen_s(&raw_file, file_path, "w");

  if (raw_file) {
    fseek(raw_file, 0, SEEK_SET);
    fwrite(file->buffer, sizeof(uint8_t), file->buffer_size, raw_file);

    fclose(raw_file);
  }
}
void file_save_binary(file_t *file, char const *file_path) {
  FILE *raw_file = 0;

  fopen_s(&raw_file, file_path, "wb");

  if (raw_file) {
    fseek(raw_file, 0, SEEK_SET);
    fwrite(file->buffer, sizeof(uint8_t), file->buffer_size, raw_file);

    fclose(raw_file);
  }
}
uint64_t file_size(file_t *file) {
  return file->buffer_size;
}
uint8_t *file_buffer(file_t *file) {
  return file->buffer;
}
void file_free(file_t *file) {
  heap_free(file->buffer);
}
