#ifndef EG_BUFFER_H
#define EG_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t buffer_create(VkBufferUsageFlags buffer_usage_flags, uint64_t buffer_size);
VkBuffer buffer_handle(buffer_t *buffer);
void buffer_destroy(buffer_t *buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_BUFFER_H
