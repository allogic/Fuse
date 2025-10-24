#ifndef EG_BUFFER_H
#define EG_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

buffer_t buffer_create(uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties);
void buffer_map(buffer_t *buffer);
void buffer_unmap(buffer_t *buffer);
void buffer_copy_to_buffer(buffer_t *buffer, buffer_t *target, VkCommandBuffer command_buffer, uint64_t buffer_copy_size);
void buffer_destroy(buffer_t *buffer);

buffer_t buffer_create_vertex(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_index(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_uniform(void *buffer, uint64_t buffer_size);
buffer_t buffer_create_storage(void *buffer, uint64_t buffer_size);

buffer_t buffer_create_vertex_coherent(uint64_t buffer_size);
buffer_t buffer_create_index_coherent(uint64_t buffer_size);
buffer_t buffer_create_uniform_coherent(uint64_t buffer_size);
buffer_t buffer_create_storage_coherent(uint64_t buffer_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_BUFFER_H
