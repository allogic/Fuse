#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

typedef struct buffer_t {
  uint64_t buffer_size;
  VkBuffer buffer;
  VkDeviceMemory device_memory;
} buffer_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t buffer_create(VkBufferUsageFlags buffer_usage_flags, void *mapped_buffer, uint64_t buffer_size);
void buffer_destroy(buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BUFFER_H
