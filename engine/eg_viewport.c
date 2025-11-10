#include <engine/eg_pch.h>
#include <engine/eg_viewport.h>

static void viewport_create_color_images(viewport_t *viewport);
static void viewport_create_depth_images(viewport_t *viewport);
static void viewport_create_frame_buffer(viewport_t *viewport);

static void viewport_destroy_color_images(viewport_t *viewport);
static void viewport_destroy_depth_images(viewport_t *viewport);
static void viewport_destroy_frame_buffer(viewport_t *viewport);

viewport_t *viewport_create(context_t *context, uint32_t width, uint32_t height) {
  viewport_t *viewport = (viewport_t *)heap_alloc(sizeof(viewport_t), 1, 0);

  uint64_t image_count = context->swapchain->image_count;

  viewport->context = context;
  viewport->width = width;
  viewport->height = height;
  viewport->color_image = (VkImage *)heap_alloc(sizeof(VkImage) * image_count, 0, 0);
  viewport->color_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * image_count, 0, 0);
  viewport->color_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * image_count, 0, 0);
  viewport->color_sampler = (VkSampler *)heap_alloc(sizeof(VkSampler) * image_count, 0, 0);
  viewport->depth_image = (VkImage *)heap_alloc(sizeof(VkImage) * image_count, 0, 0);
  viewport->depth_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * image_count, 0, 0);
  viewport->depth_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * image_count, 0, 0);
  viewport->depth_sampler = (VkSampler *)heap_alloc(sizeof(VkSampler) * image_count, 0, 0);
  viewport->frame_buffer = (VkFramebuffer *)heap_alloc(sizeof(VkFramebuffer) * image_count, 0, 0);

  viewport_create_color_images(viewport);
  viewport_create_depth_images(viewport);
  viewport_create_frame_buffer(viewport);

  while (viewport->context->viewport[viewport->link_index]) {

    viewport->link_index++;
  }

  ASSERT(viewport->link_index < 0xFF, "Invalid link index");

  viewport->context->viewport[viewport->link_index] = viewport;

  return viewport;
}
void viewport_resize(viewport_t *viewport, uint32_t width, uint32_t height) {
  viewport_destroy_frame_buffer(viewport);
  viewport_destroy_depth_images(viewport);
  viewport_destroy_color_images(viewport);

  viewport->width = width;
  viewport->height = height;

  viewport_create_color_images(viewport);
  viewport_create_depth_images(viewport);
  viewport_create_frame_buffer(viewport);
}
void viewport_destroy(viewport_t *viewport) {
  ASSERT(viewport->link_index < 0xFF, "Invalid link index");

  viewport->context->viewport[viewport->link_index] = 0;

  viewport_destroy_frame_buffer(viewport);
  viewport_destroy_depth_images(viewport);
  viewport_destroy_color_images(viewport);

  heap_free(viewport->color_image);
  heap_free(viewport->color_device_memory);
  heap_free(viewport->color_image_view);
  heap_free(viewport->color_sampler);

  heap_free(viewport->depth_image);
  heap_free(viewport->depth_device_memory);
  heap_free(viewport->depth_image_view);
  heap_free(viewport->depth_sampler);

  heap_free(viewport->frame_buffer);

  heap_free(viewport);
}

static void viewport_create_color_images(viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = viewport->width;
    image_create_info.extent.height = viewport->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = viewport->context->prefered_surface_format.format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateImage(viewport->context->device, &image_create_info, 0, &viewport->color_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(viewport->context->device, viewport->color_image[image_index], &memory_requirements);

    uint32_t memory_type_index = context_find_memory_type(viewport->context, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    VULKAN_CHECK(vkAllocateMemory(viewport->context->device, &memory_allocate_info, 0, &viewport->color_device_memory[image_index]));
    VULKAN_CHECK(vkBindImageMemory(viewport->context->device, viewport->color_image[image_index], viewport->color_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = viewport->color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = viewport->context->prefered_surface_format.format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(viewport->context->device, &image_view_create_info, 0, &viewport->color_image_view[image_index]));

    VkSamplerCreateInfo sampler_create_info = {0};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_NEAREST;
    sampler_create_info.minFilter = VK_FILTER_NEAREST;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = 1;
    sampler_create_info.maxAnisotropy = viewport->context->physical_device_properties.limits.maxSamplerAnisotropy;
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = 0;
    sampler_create_info.compareEnable = 0;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0F;
    sampler_create_info.minLod = 0.0F;
    sampler_create_info.maxLod = 0.0F;

    VULKAN_CHECK(vkCreateSampler(viewport->context->device, &sampler_create_info, 0, &viewport->color_sampler[image_index]));

    image_index++;
  }
}
static void viewport_create_depth_images(viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = viewport->width;
    image_create_info.extent.height = viewport->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = viewport->context->swapchain->depth_format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateImage(viewport->context->device, &image_create_info, 0, &viewport->depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(viewport->context->device, viewport->depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = context_find_memory_type(viewport->context, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    VULKAN_CHECK(vkAllocateMemory(viewport->context->device, &memory_allocate_info, 0, &viewport->depth_device_memory[image_index]));
    VULKAN_CHECK(vkBindImageMemory(viewport->context->device, viewport->depth_image[image_index], viewport->depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = viewport->depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = viewport->context->swapchain->depth_format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(viewport->context->device, &image_view_create_info, 0, &viewport->depth_image_view[image_index]));

    VkSamplerCreateInfo sampler_create_info = {0};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_NEAREST;
    sampler_create_info.minFilter = VK_FILTER_NEAREST;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = 1;
    sampler_create_info.maxAnisotropy = viewport->context->physical_device_properties.limits.maxSamplerAnisotropy;
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = 0;
    sampler_create_info.compareEnable = 0;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0F;
    sampler_create_info.minLod = 0.0F;
    sampler_create_info.maxLod = 0.0F;

    VULKAN_CHECK(vkCreateSampler(viewport->context->device, &sampler_create_info, 0, &viewport->depth_sampler[image_index]));

    image_index++;
  }
}
static void viewport_create_frame_buffer(viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {viewport->color_image_view[image_index], viewport->depth_image_view[image_index]};

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = viewport->context->renderer->gbuffer_render_pass;
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = viewport->width;
    frame_buffer_create_info.height = viewport->height;
    frame_buffer_create_info.layers = 1;

    VULKAN_CHECK(vkCreateFramebuffer(viewport->context->device, &frame_buffer_create_info, 0, &viewport->frame_buffer[image_index]));

    image_index++;
  }
}

static void viewport_destroy_color_images(viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    vkDestroySampler(viewport->context->device, viewport->color_sampler[image_index], 0);
    vkDestroyImageView(viewport->context->device, viewport->color_image_view[image_index], 0);
    vkFreeMemory(viewport->context->device, viewport->color_device_memory[image_index], 0);
    vkDestroyImage(viewport->context->device, viewport->color_image[image_index], 0);

    image_index++;
  }
}
static void viewport_destroy_depth_images(viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    vkDestroySampler(viewport->context->device, viewport->depth_sampler[image_index], 0);
    vkDestroyImageView(viewport->context->device, viewport->depth_image_view[image_index], 0);
    vkFreeMemory(viewport->context->device, viewport->depth_device_memory[image_index], 0);
    vkDestroyImage(viewport->context->device, viewport->depth_image[image_index], 0);

    image_index++;
  }
}
static void viewport_destroy_frame_buffer(viewport_t *viewport) {
  // VULKAN_CHECK(vkQueueWaitIdle(viewport->context->graphic_queue)); // TODO

  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(viewport->context->device, viewport->frame_buffer[image_index], 0);

    image_index++;
  }
}
