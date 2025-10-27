#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>

buffer_t *buffer_create(context_t *context, uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties) {
  buffer_t *buffer = (buffer_t *)heap_alloc(sizeof(buffer_t), 1, 0);

  buffer->context = context;
  buffer->size = buffer_size;

  VkBufferCreateInfo buffer_create_info = {0};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = buffer_size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VULKAN_CHECK(vkCreateBuffer(buffer->context->device, &buffer_create_info, 0, &buffer->handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(buffer->context->device, buffer->handle, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info = {0};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = context_find_memory_type(buffer->context, memory_requirements.memoryTypeBits, memory_properties);

  VULKAN_CHECK(vkAllocateMemory(buffer->context->device, &memory_allocate_info, 0, &buffer->device_memory));
  VULKAN_CHECK(vkBindBufferMemory(buffer->context->device, buffer->handle, buffer->device_memory, 0));

  return buffer;
}
void buffer_map(buffer_t *buffer) {
  VULKAN_CHECK(vkMapMemory(buffer->context->device, buffer->device_memory, 0, buffer->size, 0, &buffer->mapped_memory));
}
void buffer_unmap(buffer_t *buffer) {
  vkUnmapMemory(buffer->context->device, buffer->device_memory);

  buffer->mapped_memory = 0;
}
void buffer_copy_to_buffer(buffer_t *buffer, buffer_t *target, VkCommandBuffer command_buffer, uint64_t buffer_copy_size) {
  VkBufferCopy buffer_copy = {0};

  buffer_copy.size = buffer_copy_size;

  vkCmdCopyBuffer(command_buffer, buffer->handle, target->handle, 1, &buffer_copy);
}
void buffer_destroy(buffer_t *buffer) {
  if (buffer->mapped_memory) {
    vkUnmapMemory(buffer->context->device, buffer->device_memory);
  }

  vkFreeMemory(buffer->context->device, buffer->device_memory, 0);

  vkDestroyBuffer(buffer->context->device, buffer->handle, 0);

  heap_free(buffer);
}

buffer_t *buffer_create_vertex(context_t *context, void *buffer, uint64_t buffer_size) {
  buffer_t *staging_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer(context);
  buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(context, command_buffer);

  buffer_destroy(staging_buffer);

  return target_buffer;
}
buffer_t *buffer_create_index(context_t *context, void *buffer, uint64_t buffer_size) {
  buffer_t *staging_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer(context);
  buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(context, command_buffer);

  buffer_destroy(staging_buffer);

  return target_buffer;
}
buffer_t *buffer_create_uniform(context_t *context, void *buffer, uint64_t buffer_size) {
  buffer_t *staging_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer(context);
  buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(context, command_buffer);

  buffer_destroy(staging_buffer);

  return target_buffer;
}
buffer_t *buffer_create_storage(context_t *context, void *buffer, uint64_t buffer_size) {
  buffer_t *staging_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = context_begin_command_buffer(context);
  buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer, buffer_size);
  context_end_command_buffer(context, command_buffer);

  buffer_destroy(staging_buffer);

  return target_buffer;
}

buffer_t *buffer_create_vertex_coherent(context_t *context, uint64_t buffer_size) {
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(target_buffer);

  return target_buffer;
}
buffer_t *buffer_create_index_coherent(context_t *context, uint64_t buffer_size) {
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(target_buffer);

  return target_buffer;
}
buffer_t *buffer_create_uniform_coherent(context_t *context, uint64_t buffer_size) {
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(target_buffer);

  return target_buffer;
}
buffer_t *buffer_create_storage_coherent(context_t *context, uint64_t buffer_size) {
  buffer_t *target_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  buffer_map(target_buffer);

  return target_buffer;
}
