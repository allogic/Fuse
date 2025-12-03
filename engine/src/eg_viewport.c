#include <engine/eg_pch.h>

struct eg_viewport_t {
  uint64_t index;
  uint32_t width;
  uint32_t height;
  VkImage *color_image;
  VkDeviceMemory *color_device_memory;
  VkImageView *color_image_view;
  VkSampler *color_sampler;
  VkImage *depth_image;
  VkDeviceMemory *depth_device_memory;
  VkImageView *depth_image_view;
  VkSampler *depth_sampler;
  VkFramebuffer *frame_buffer;
};

static void eg_viewport_create_color_images(eg_viewport_t *viewport);
static void eg_viewport_create_depth_images(eg_viewport_t *viewport);
static void eg_viewport_create_frame_buffer(eg_viewport_t *viewport);

static void eg_viewport_destroy_color_images(eg_viewport_t *viewport);
static void eg_viewport_destroy_depth_images(eg_viewport_t *viewport);
static void eg_viewport_destroy_frame_buffer(eg_viewport_t *viewport);

eg_viewport_t *eg_viewport_create(uint32_t width, uint32_t height) {
  eg_viewport_t *viewport = (eg_viewport_t *)eg_heap_alloc(sizeof(eg_viewport_t), 1, 0);

  eg_context_set_viewport(viewport, &viewport->index);

  uint32_t image_count = eg_swapchain_image_count();

  viewport->width = width;
  viewport->height = height;
  viewport->color_image = (VkImage *)eg_heap_alloc(sizeof(VkImage) * image_count, 0, 0);
  viewport->color_device_memory = (VkDeviceMemory *)eg_heap_alloc(sizeof(VkDeviceMemory) * image_count, 0, 0);
  viewport->color_image_view = (VkImageView *)eg_heap_alloc(sizeof(VkImageView) * image_count, 0, 0);
  viewport->color_sampler = (VkSampler *)eg_heap_alloc(sizeof(VkSampler) * image_count, 0, 0);
  viewport->depth_image = (VkImage *)eg_heap_alloc(sizeof(VkImage) * image_count, 0, 0);
  viewport->depth_device_memory = (VkDeviceMemory *)eg_heap_alloc(sizeof(VkDeviceMemory) * image_count, 0, 0);
  viewport->depth_image_view = (VkImageView *)eg_heap_alloc(sizeof(VkImageView) * image_count, 0, 0);
  viewport->depth_sampler = (VkSampler *)eg_heap_alloc(sizeof(VkSampler) * image_count, 0, 0);
  viewport->frame_buffer = (VkFramebuffer *)eg_heap_alloc(sizeof(VkFramebuffer) * image_count, 0, 0);

  eg_viewport_create_color_images(viewport);
  eg_viewport_create_depth_images(viewport);
  eg_viewport_create_frame_buffer(viewport);

  return viewport;
}
void eg_viewport_resize(eg_viewport_t *viewport, uint32_t width, uint32_t height) {
  eg_viewport_destroy_frame_buffer(viewport);
  eg_viewport_destroy_depth_images(viewport);
  eg_viewport_destroy_color_images(viewport);

  viewport->width = width;
  viewport->height = height;

  eg_viewport_create_color_images(viewport);
  eg_viewport_create_depth_images(viewport);
  eg_viewport_create_frame_buffer(viewport);
}
void eg_viewport_destroy(eg_viewport_t *viewport) {
  eg_viewport_t **viewports = eg_context_viewports();

  viewports[viewport->index] = 0;

  eg_viewport_destroy_frame_buffer(viewport);
  eg_viewport_destroy_depth_images(viewport);
  eg_viewport_destroy_color_images(viewport);

  eg_heap_free(viewport->color_image);
  eg_heap_free(viewport->color_device_memory);
  eg_heap_free(viewport->color_image_view);
  eg_heap_free(viewport->color_sampler);

  eg_heap_free(viewport->depth_image);
  eg_heap_free(viewport->depth_device_memory);
  eg_heap_free(viewport->depth_image_view);
  eg_heap_free(viewport->depth_sampler);

  eg_heap_free(viewport->frame_buffer);

  eg_heap_free(viewport);
}

uint32_t eg_viewport_width(eg_viewport_t *viewport) {
  return viewport->width;
}
uint32_t eg_viewport_height(eg_viewport_t *viewport) {
  return viewport->height;
}
VkImage eg_viewport_color_image(eg_viewport_t *viewport, uint32_t index) {
  return viewport->color_image[index];
}
VkImage eg_viewport_depth_image(eg_viewport_t *viewport, uint32_t index) {
  return viewport->depth_image[index];
}
VkFramebuffer eg_viewport_frame_buffer(eg_viewport_t *viewport, uint32_t index) {
  return viewport->frame_buffer[index];
}
VkSampler eg_viewport_color_sampler(eg_viewport_t *viewport, uint32_t index) {
  return viewport->color_sampler[index];
}
VkSampler eg_viewport_depth_sampler(eg_viewport_t *viewport, uint32_t index) {
  return viewport->depth_sampler[index];
}
VkImageView eg_viewport_color_image_view(eg_viewport_t *viewport, uint32_t index) {
  return viewport->color_image_view[index];
}
VkImageView eg_viewport_depth_image_view(eg_viewport_t *viewport, uint32_t index) {
  return viewport->depth_image_view[index];
}

static void eg_viewport_create_color_images(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = viewport->width;
    image_create_info.extent.height = viewport->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = eg_context_surface_image_color_format();
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    EG_VULKAN_CHECK(vkCreateImage(eg_context_device(), &image_create_info, 0, &viewport->color_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(eg_context_device(), viewport->color_image[image_index], &memory_requirements);

    uint32_t memory_type_index = eg_context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    EG_VULKAN_CHECK(vkAllocateMemory(eg_context_device(), &memory_allocate_info, 0, &viewport->color_device_memory[image_index]));
    EG_VULKAN_CHECK(vkBindImageMemory(eg_context_device(), viewport->color_image[image_index], viewport->color_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = viewport->color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = eg_context_surface_image_color_format();
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(eg_context_device(), &image_view_create_info, 0, &viewport->color_image_view[image_index]));

    VkSamplerCreateInfo sampler_create_info = {0};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_NEAREST;
    sampler_create_info.minFilter = VK_FILTER_NEAREST;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = 1;
    sampler_create_info.maxAnisotropy = eg_context_max_anisotropy();
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = 0;
    sampler_create_info.compareEnable = 0;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0F;
    sampler_create_info.minLod = 0.0F;
    sampler_create_info.maxLod = 0.0F;

    EG_VULKAN_CHECK(vkCreateSampler(eg_context_device(), &sampler_create_info, 0, &viewport->color_sampler[image_index]));

    image_index++;
  }
}
static void eg_viewport_create_depth_images(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = viewport->width;
    image_create_info.extent.height = viewport->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = eg_context_surface_image_depth_format();
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    EG_VULKAN_CHECK(vkCreateImage(eg_context_device(), &image_create_info, 0, &viewport->depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(eg_context_device(), viewport->depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = eg_context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    EG_VULKAN_CHECK(vkAllocateMemory(eg_context_device(), &memory_allocate_info, 0, &viewport->depth_device_memory[image_index]));
    EG_VULKAN_CHECK(vkBindImageMemory(eg_context_device(), viewport->depth_image[image_index], viewport->depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = viewport->depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = eg_context_surface_image_depth_format();
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(eg_context_device(), &image_view_create_info, 0, &viewport->depth_image_view[image_index]));

    VkSamplerCreateInfo sampler_create_info = {0};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = VK_FILTER_NEAREST;
    sampler_create_info.minFilter = VK_FILTER_NEAREST;
    sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_create_info.anisotropyEnable = 1;
    sampler_create_info.maxAnisotropy = eg_context_max_anisotropy();
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_create_info.unnormalizedCoordinates = 0;
    sampler_create_info.compareEnable = 0;
    sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.mipLodBias = 0.0F;
    sampler_create_info.minLod = 0.0F;
    sampler_create_info.maxLod = 0.0F;

    EG_VULKAN_CHECK(vkCreateSampler(eg_context_device(), &sampler_create_info, 0, &viewport->depth_sampler[image_index]));

    image_index++;
  }
}
static void eg_viewport_create_frame_buffer(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    VkImageView image_attachments[] = {
      viewport->color_image_view[image_index],
      viewport->depth_image_view[image_index],
    };

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = eg_renderpass_gbuffer();
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = EG_ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = viewport->width;
    frame_buffer_create_info.height = viewport->height;
    frame_buffer_create_info.layers = 1;

    EG_VULKAN_CHECK(vkCreateFramebuffer(eg_context_device(), &frame_buffer_create_info, 0, &viewport->frame_buffer[image_index]));

    image_index++;
  }
}

static void eg_viewport_destroy_color_images(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    vkDestroySampler(eg_context_device(), viewport->color_sampler[image_index], 0);
    vkDestroyImageView(eg_context_device(), viewport->color_image_view[image_index], 0);
    vkFreeMemory(eg_context_device(), viewport->color_device_memory[image_index], 0);
    vkDestroyImage(eg_context_device(), viewport->color_image[image_index], 0);

    image_index++;
  }
}
static void eg_viewport_destroy_depth_images(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    vkDestroySampler(eg_context_device(), viewport->depth_sampler[image_index], 0);
    vkDestroyImageView(eg_context_device(), viewport->depth_image_view[image_index], 0);
    vkFreeMemory(eg_context_device(), viewport->depth_device_memory[image_index], 0);
    vkDestroyImage(eg_context_device(), viewport->depth_image[image_index], 0);

    image_index++;
  }
}
static void eg_viewport_destroy_frame_buffer(eg_viewport_t *viewport) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    vkDestroyFramebuffer(eg_context_device(), viewport->frame_buffer[image_index], 0);

    image_index++;
  }
}
