#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>

buffer_t buffer_create(VkBufferUsageFlags buffer_usage_flags, void *mapped_buffer, uint64_t buffer_size) {
  buffer_t buffer = {0};

  buffer.buffer_size = buffer_size;

  VkBufferCreateInfo buffer_create_info = {0};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = buffer_size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VULKAN_CHECK(vkCreateBuffer(g_globals.context_device, &buffer_create_info, 0, &buffer.buffer));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(g_globals.context_device, buffer.buffer, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info = {0};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

  VULKAN_CHECK(vkAllocateMemory(g_globals.context_device, &memory_allocate_info, 0, &buffer.device_memory));
  VULKAN_CHECK(vkBindBufferMemory(g_globals.context_device, buffer.buffer, buffer.device_memory, 0));

  VULKAN_CHECK(vkMapMemory(g_globals.context_device, buffer.device_memory, 0, buffer.buffer_size, 0, &mapped_buffer));

  return buffer;
}
VkBuffer buffer_handle(buffer_t *buffer) {
  return buffer->buffer;
}
void buffer_destroy(buffer_t *buffer) {
  vkUnmapMemory(g_globals.context_device, buffer->device_memory);

  vkFreeMemory(g_globals.context_device, buffer->device_memory, 0);

  vkDestroyBuffer(g_globals.context_device, buffer->buffer, 0);
}
