#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

typedef void *buffer_value_t;

typedef struct buffer_t {
  vector_t buffers;
  vector_t device_memory;
  vector_t raw_buffers;
} buffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t *buffer_create(int32_t frames_in_flight);
void buffer_destroy(buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BUFFER_H
