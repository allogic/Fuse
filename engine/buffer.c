#include <library/core/api.h>

#include <engine/buffer.h>
#include <engine/context.h>
#include <engine/macros.h>

buffer_t buffer_create(VkBufferUsageFlags buffer_usage_flags, int32_t frames_in_flight, void *raw_buffer, uint64_t buffer_size) {
  buffer_t buffer = {0};

  buffer.buffers = vector_create(sizeof(VkBuffer));
  buffer.device_memory = vector_create(sizeof(VkDeviceMemory));
  buffer.raw_buffers = vector_create(sizeof(buffer_value_t));

  vector_resize(&buffer.buffers, frames_in_flight);
  vector_resize(&buffer.device_memory, frames_in_flight);
  vector_resize(&buffer.raw_buffers, frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < frames_in_flight) {

    VkBuffer *vulkan_buffer = (VkBuffer *)vector_at(&buffer.buffers, frame_index);
    VkDeviceMemory *vulkan_device_memory = (VkDeviceMemory *)vector_at(&buffer.device_memory, frame_index);
    buffer_value_t *raw_buffer = (buffer_value_t *)vector_at(&buffer.raw_buffers, frame_index);

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = buffer_size;
    buffer_create_info.usage = buffer_usage_flags;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, vulkan_buffer));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, vulkan_buffer, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, vulkan_device_memory));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, vulkan_buffer, vulkan_device_memory, 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, vulkan_device_memory, 0, buffer_size, 0, raw_buffer));

    frame_index++;
  }
}
void buffer_destroy(buffer_t *buffer) {
  vector_destroy(&buffer->buffers);
  vector_destroy(&buffer->device_memory);
  vector_destroy(&buffer->raw_buffers);

  /*
  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_debug_line_index_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_debug_line_index_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_debug_line_index_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_debug_line_index_buffer);
  heap_free(s_renderer_debug_line_index_buffer_device_memory);
  heap_free(s_renderer_debug_line_index);
  */
}
