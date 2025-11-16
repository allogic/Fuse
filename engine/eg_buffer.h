#ifndef EG_BUFFER_H
#define EG_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_buffer_t *eg_buffer_create(eg_context_t *context, uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties);
void eg_buffer_map(eg_buffer_t *buffer);
void eg_buffer_unmap(eg_buffer_t *buffer);
void eg_buffer_copy_to_buffer(eg_buffer_t *buffer, eg_buffer_t *target, VkCommandBuffer command_buffer);
void eg_buffer_copy_to_image(eg_buffer_t *buffer, eg_image_t *target, VkCommandBuffer command_buffer);
void eg_buffer_destroy(eg_buffer_t *buffer);

eg_buffer_t *eg_buffer_create_vertex(eg_context_t *context, void *buffer, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_index(eg_context_t *context, void *buffer, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_uniform(eg_context_t *context, void *buffer, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_storage(eg_context_t *context, void *buffer, uint64_t buffer_size);

eg_buffer_t *eg_buffer_create_vertex_coherent(eg_context_t *context, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_index_coherent(eg_context_t *context, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_uniform_coherent(eg_context_t *context, uint64_t buffer_size);
eg_buffer_t *eg_buffer_create_storage_coherent(eg_context_t *context, uint64_t buffer_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_BUFFER_H
