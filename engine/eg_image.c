#include <engine/eg_pch.h>
#include <engine/eg_image.h>

image_t *image_create(context_t *context, uint32_t width, uint32_t height, uint32_t depth, uint32_t channels, VkImageType type, VkImageViewType view_type, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImageAspectFlags aspect_flags, VkFormat format, VkImageTiling tiling, VkFilter filter) {
  image_t *image = (image_t *)heap_alloc(sizeof(image_t), 1, 0);

  image->context = context;
  image->size = width * height * depth * channels;
  image->width = width;
  image->height = height;
  image->depth = depth;
  image->channels = channels;
  image->usage = usage;
  image->aspect_flags = aspect_flags;
  image->type = type;
  image->view_type = view_type;
  image->format = format;
  image->tiling = tiling;
  image->filter = filter;

  VkImageCreateInfo image_create_info = {0};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = image->type;
  image_create_info.extent.width = image->width;
  image_create_info.extent.height = image->height;
  image_create_info.extent.depth = image->depth;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.format = image->format;
  image_create_info.tiling = image->tiling;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage = image->usage;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VULKAN_CHECK(vkCreateImage(image->context->device, &image_create_info, 0, &image->handle));

  VkMemoryRequirements memory_requirements = {0};

  vkGetImageMemoryRequirements(image->context->device, image->handle, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info = {0};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = context_find_memory_type(image->context, memory_requirements.memoryTypeBits, memory_properties);

  VULKAN_CHECK(vkAllocateMemory(image->context->device, &memory_allocate_info, 0, &image->device_memory));
  VULKAN_CHECK(vkBindImageMemory(image->context->device, image->handle, image->device_memory, 0));

  return image;
}
void image_map(image_t *image) {
  VULKAN_CHECK(vkMapMemory(image->context->device, image->device_memory, 0, image->size, 0, &image->mapped_memory));
}
void image_unmap(image_t *image) {
  vkUnmapMemory(image->context->device, image->device_memory);

  image->mapped_memory = 0;
}
void image_copy_to_image(image_t *image, image_t *target, VkCommandBuffer command_buffer) {
  VkImageCopy image_copy = {0};
  image_copy.extent.width = image->width;
  image_copy.extent.height = image->height;
  image_copy.extent.depth = image->depth;

  // TODO: insert layout transition checks..

  vkCmdCopyImage(command_buffer, image->handle, VK_IMAGE_LAYOUT_UNDEFINED, target->handle, VK_IMAGE_LAYOUT_UNDEFINED, 1, &image_copy);
}
void image_copy_to_buffer(image_t *image, buffer_t *target, VkCommandBuffer command_buffer) {
  VkBufferImageCopy buffer_image_copy = {0};
  buffer_image_copy.bufferOffset = 0;
  buffer_image_copy.bufferRowLength = 0;
  buffer_image_copy.bufferImageHeight = 0;
  buffer_image_copy.imageSubresource.aspectMask = image->aspect_flags;
  buffer_image_copy.imageSubresource.mipLevel = 0;
  buffer_image_copy.imageSubresource.baseArrayLayer = 0;
  buffer_image_copy.imageSubresource.layerCount = 1;
  buffer_image_copy.imageOffset.x = 0;
  buffer_image_copy.imageOffset.y = 0;
  buffer_image_copy.imageOffset.z = 0;
  buffer_image_copy.imageExtent.width = image->width;
  buffer_image_copy.imageExtent.height = image->height;
  buffer_image_copy.imageExtent.depth = image->depth;

  // TODO: insert layout transition checks..

  vkCmdCopyImageToBuffer(command_buffer, image->handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, target->handle, 1, &buffer_image_copy);
}
void image_destroy(image_t *image) {
  if (image->mapped_memory) {
    vkUnmapMemory(image->context->device, image->device_memory);
  }

  vkFreeMemory(image->context->device, image->device_memory, 0);

  vkDestroyImage(image->context->device, image->handle, 0);

  heap_free(image);
}

VkImageView image_create_view(context_t *context, VkImage image, VkImageViewType view_type, VkImageAspectFlags aspect_flags, VkFormat format) {
  VkImageView image_view = 0;

  VkImageViewCreateInfo image_view_create_info = {0};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.image = image;
  image_view_create_info.viewType = view_type;
  image_view_create_info.format = format;
  image_view_create_info.subresourceRange.aspectMask = aspect_flags;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  VULKAN_CHECK(vkCreateImageView(context->device, &image_view_create_info, 0, &image_view));

  return image_view;
}
VkSampler image_create_sampler(context_t *context, VkImage image, VkFilter filter) {
  VkSampler sampler = 0;

  VkSamplerCreateInfo sampler_create_info = {0};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.magFilter = filter;
  sampler_create_info.minFilter = filter;
  sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.anisotropyEnable = 1;
  sampler_create_info.maxAnisotropy = context->physical_device_properties.limits.maxSamplerAnisotropy;
  sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_create_info.unnormalizedCoordinates = 0;
  sampler_create_info.compareEnable = 0;
  sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_create_info.mipLodBias = 0.0F;
  sampler_create_info.minLod = 0.0F;
  sampler_create_info.maxLod = 0.0F;

  VULKAN_CHECK(vkCreateSampler(context->device, &sampler_create_info, 0, &sampler));

  return sampler;
}
void image_layout_transition(context_t *context, VkImage image, VkCommandBuffer command_buffer, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask) {
  VkImageMemoryBarrier image_memory_barrier = {0};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.oldLayout = old_layout;
  image_memory_barrier.newLayout = new_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = image;
  image_memory_barrier.subresourceRange.aspectMask = aspect_mask;
  image_memory_barrier.subresourceRange.baseMipLevel = 0;
  image_memory_barrier.subresourceRange.levelCount = 1;
  image_memory_barrier.subresourceRange.baseArrayLayer = 0;
  image_memory_barrier.subresourceRange.layerCount = 1;
  image_memory_barrier.srcAccessMask = VK_ACCESS_NONE;
  image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(
    command_buffer,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    0, 0, 0, 0, 0, 1, &image_memory_barrier);
}

image_t *image_create_2d(context_t *context, void *buffer, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter) {
  uint64_t buffer_size = width * height;

  buffer_t *staging_buffer = buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  image_t *target_image = image_create(context, width, height, 1, channels, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, format, tiling, filter);

  buffer_map(staging_buffer);
  memcpy(staging_buffer->mapped_memory, buffer, buffer_size);
  buffer_unmap(staging_buffer);

  // TODO
  VkCommandBuffer command_buffer = context_begin_command_buffer(context);
  // targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  buffer_copy_to_image(staging_buffer, target_image, command_buffer);
  // targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
  context_end_command_buffer(context, command_buffer);

  return target_image;
}
image_t *image_create_2d_depth_stencil(context_t *context, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter) {
  return image_create(context, width, height, 1, channels, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, format, tiling, filter);
}
