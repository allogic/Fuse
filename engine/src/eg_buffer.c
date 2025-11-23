#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>

eg_buffer_t *eg_buffer_create(eg_context_t *context, uint64_t buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties) {
  eg_buffer_t *buffer = (eg_buffer_t *)lb_heap_alloc(sizeof(eg_buffer_t), 1, 0);

  buffer->context = context;
  buffer->size = buffer_size;

  VkBufferCreateInfo buffer_create_info = {0};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = buffer_size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  EG_VULKAN_CHECK(vkCreateBuffer(buffer->context->device, &buffer_create_info, 0, &buffer->handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetBufferMemoryRequirements(buffer->context->device, buffer->handle, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info = {0};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = eg_context_find_memory_type(buffer->context, memory_requirements.memoryTypeBits, memory_properties);

  EG_VULKAN_CHECK(vkAllocateMemory(buffer->context->device, &memory_allocate_info, 0, &buffer->device_memory));
  EG_VULKAN_CHECK(vkBindBufferMemory(buffer->context->device, buffer->handle, buffer->device_memory, 0));

  return buffer;
}
void eg_buffer_map(eg_buffer_t *buffer) {
  EG_VULKAN_CHECK(vkMapMemory(buffer->context->device, buffer->device_memory, 0, buffer->size, 0, &buffer->mapped_memory));
}
void eg_buffer_unmap(eg_buffer_t *buffer) {
  vkUnmapMemory(buffer->context->device, buffer->device_memory);

  buffer->mapped_memory = 0;
}
void eg_buffer_copy_to_buffer(eg_buffer_t *buffer, eg_buffer_t *target, VkCommandBuffer command_buffer) {
  VkBufferCopy buffer_copy = {0};
  buffer_copy.srcOffset = 0;
  buffer_copy.dstOffset = 0;
  buffer_copy.size = target->size;

  vkCmdCopyBuffer(command_buffer, buffer->handle, target->handle, 1, &buffer_copy);
}
void eg_buffer_copy_to_image(eg_buffer_t *buffer, eg_image_t *target, VkCommandBuffer command_buffer) {
  VkBufferImageCopy buffer_image_copy = {0};
  buffer_image_copy.bufferOffset = 0;
  buffer_image_copy.bufferRowLength = 0;
  buffer_image_copy.bufferImageHeight = 0;
  buffer_image_copy.imageSubresource.aspectMask = target->aspect_flags;
  buffer_image_copy.imageSubresource.mipLevel = 0;
  buffer_image_copy.imageSubresource.baseArrayLayer = 0;
  buffer_image_copy.imageSubresource.layerCount = 1;
  buffer_image_copy.imageOffset.x = 0;
  buffer_image_copy.imageOffset.y = 0;
  buffer_image_copy.imageOffset.z = 0;
  buffer_image_copy.imageExtent.width = target->width;
  buffer_image_copy.imageExtent.height = target->height;
  buffer_image_copy.imageExtent.depth = target->depth;

  // TODO: insert layout transition checks..

  vkCmdCopyBufferToImage(command_buffer, buffer->handle, target->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
}
void eg_buffer_destroy(eg_buffer_t *buffer) {
  if (buffer->mapped_memory) {
    vkUnmapMemory(buffer->context->device, buffer->device_memory);
  }

  vkFreeMemory(buffer->context->device, buffer->device_memory, 0);

  vkDestroyBuffer(buffer->context->device, buffer->handle, 0);

  lb_heap_free(buffer);
}

eg_buffer_t *eg_buffer_create_vertex(eg_context_t *context, void *buffer, uint64_t buffer_size) {
  eg_buffer_t *staging_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  eg_buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  eg_buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = eg_context_begin_command_buffer(context);
  eg_buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer);
  eg_context_end_command_buffer(context, command_buffer);

  eg_buffer_destroy(staging_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_index(eg_context_t *context, void *buffer, uint64_t buffer_size) {
  eg_buffer_t *staging_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  eg_buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  eg_buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = eg_context_begin_command_buffer(context);
  eg_buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer);
  eg_context_end_command_buffer(context, command_buffer);

  eg_buffer_destroy(staging_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_uniform(eg_context_t *context, void *buffer, uint64_t buffer_size) {
  eg_buffer_t *staging_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  eg_buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  eg_buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = eg_context_begin_command_buffer(context);
  eg_buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer);
  eg_context_end_command_buffer(context, command_buffer);

  eg_buffer_destroy(staging_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_storage(eg_context_t *context, void *buffer, uint64_t buffer_size) {
  eg_buffer_t *staging_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  eg_buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  eg_buffer_unmap(staging_buffer);

  VkCommandBuffer command_buffer = eg_context_begin_command_buffer(context);
  eg_buffer_copy_to_buffer(target_buffer, target_buffer, command_buffer);
  eg_context_end_command_buffer(context, command_buffer);

  eg_buffer_destroy(staging_buffer);

  return target_buffer;
}

eg_buffer_t *eg_buffer_create_vertex_coherent(eg_context_t *context, uint64_t buffer_size) {
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  eg_buffer_map(target_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_index_coherent(eg_context_t *context, uint64_t buffer_size) {
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  eg_buffer_map(target_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_uniform_coherent(eg_context_t *context, uint64_t buffer_size) {
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  eg_buffer_map(target_buffer);

  return target_buffer;
}
eg_buffer_t *eg_buffer_create_storage_coherent(eg_context_t *context, uint64_t buffer_size) {
  eg_buffer_t *target_buffer = eg_buffer_create(context, buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  eg_buffer_map(target_buffer);

  return target_buffer;
}
