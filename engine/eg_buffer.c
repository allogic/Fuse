#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>

buffer_t buffer_create(uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties) {
  buffer_t buffer = {0};

  buffer.buffer_size = buffer_size;

  VkBufferCreateInfo buffer_create_info = {0};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = buffer_size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &buffer.buffer));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(g_context_device, buffer.buffer, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info = {0};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, memory_properties);

  VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &buffer.device_memory));
  VULKAN_CHECK(vkBindBufferMemory(g_context_device, buffer.buffer, buffer.device_memory, 0));

  return buffer;
}
void buffer_map(buffer_t *buffer) {
  VULKAN_CHECK(vkMapMemory(g_context_device, buffer->device_memory, 0, buffer->buffer_size, 0, &buffer->mapped_memory));
}
void buffer_unmap(buffer_t *buffer) {
  vkUnmapMemory(g_context_device, buffer->device_memory);

  buffer->mapped_memory = 0;
}
void buffer_copy_to_buffer(buffer_t *buffer, buffer_t *target, VkCommandBuffer command_buffer, uint64_t buffer_copy_size) {
  VkBufferCopy buffer_copy = {0};

  buffer_copy.size = buffer_copy_size;

  vkCmdCopyBuffer(command_buffer, buffer->buffer, target->buffer, 1, &buffer_copy);
}
void buffer_destroy(buffer_t *buffer) {
  if (buffer->device_memory) {
    vkUnmapMemory(g_context_device, buffer->device_memory);
  }

  vkFreeMemory(g_context_device, buffer->device_memory, 0);

  vkDestroyBuffer(g_context_device, buffer->buffer, 0);
}

buffer_t buffer_create_vertex(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(&staging_buffer);
  memcpy(staging_buffer.mapped_memory, buffer, buffer_size);
  buffer_unmap(&staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer();
  buffer_copy_to_buffer(&target_buffer, &target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_index(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(&staging_buffer);
  memcpy(staging_buffer.mapped_memory, buffer, buffer_size);
  buffer_unmap(&staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer();
  buffer_copy_to_buffer(&target_buffer, &target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_uniform(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(&staging_buffer);
  memcpy(staging_buffer.mapped_memory, buffer, buffer_size);
  buffer_unmap(&staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer();
  buffer_copy_to_buffer(&target_buffer, &target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}
buffer_t buffer_create_storage(void *buffer, uint64_t buffer_size) {
  buffer_t staging_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(&staging_buffer);
  memcpy(staging_buffer.mapped_memory, buffer, buffer_size);
  buffer_unmap(&staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer();
  buffer_copy_to_buffer(&target_buffer, &target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(command_buffer);

  buffer_destroy(&staging_buffer);

  return target_buffer;
}

buffer_t buffer_create_vertex_coherent(uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  return target_buffer;
}
buffer_t buffer_create_index_coherent(uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  return target_buffer;
}
buffer_t buffer_create_uniform_coherent(uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  return target_buffer;
}
buffer_t buffer_create_storage_coherent(uint64_t buffer_size) {
  buffer_t target_buffer = buffer_create(buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(&target_buffer);

  return target_buffer;
}
