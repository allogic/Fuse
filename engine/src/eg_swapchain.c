#include <engine/eg_pch.h>

struct eg_swapchain_t {
  eg_swapchain_asset_t asset;
  uint8_t is_dirty;
  uint32_t image_count;
  VkSwapchainKHR handle;
  VkImage *color_image;
  VkImageView *color_image_view;
  VkImage *depth_image;
  VkDeviceMemory *depth_device_memory;
  VkImageView *depth_image_view;
  VkFramebuffer *frame_buffer;
};

static void eg_swapchain_create_color_images(void);
static void eg_swapchain_create_depth_images(void);
static void eg_swapchain_create_frame_buffer(void);

static void eg_swapchain_destroy_color_images(void);
static void eg_swapchain_destroy_depth_images(void);
static void eg_swapchain_destroy_frame_buffer(void);

static eg_swapchain_t *s_swapchain_current = 0;

eg_swapchain_t *eg_swapchain_current(void) {
  return s_swapchain_current;
}

void eg_swapchain_create(eg_swapchain_asset_id_t swapchain_asset_id) {
  s_swapchain_current = (eg_swapchain_t *)eg_heap_alloc(sizeof(eg_swapchain_t), 1, 0);

  uint32_t min_image_count = eg_context_surface_min_image_count();
  uint32_t max_image_count = eg_context_surface_max_image_count();

  s_swapchain_current->asset = eg_database_load_swapchain_asset_by_id(swapchain_asset_id);
  s_swapchain_current->image_count = s_swapchain_current->asset.image_count;
  s_swapchain_current->image_count = EG_MAX(s_swapchain_current->image_count, min_image_count);
  s_swapchain_current->image_count = EG_MIN(s_swapchain_current->image_count, max_image_count);

  uint32_t window_width = eg_context_window_width();
  uint32_t window_height = eg_context_window_height();

  VkSwapchainCreateInfoKHR swapchain_create_info = {0};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.surface = eg_context_surface();
  swapchain_create_info.minImageCount = s_swapchain_current->image_count;
  swapchain_create_info.imageFormat = eg_context_surface_image_color_format();
  swapchain_create_info.imageColorSpace = eg_context_surface_image_color_space();
  swapchain_create_info.imageExtent.width = window_width;
  swapchain_create_info.imageExtent.height = window_height;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_create_info.preTransform = eg_context_surface_transform();
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = eg_context_surface_present_mode();
  swapchain_create_info.clipped = 1;
  swapchain_create_info.oldSwapchain = 0;

  int32_t primary_queue_index = eg_context_primary_queue_index();
  int32_t present_queue_index = eg_context_present_queue_index();

  if (primary_queue_index == present_queue_index) {

    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = 0;

  } else {

    int32_t queue_families[2] = {
      primary_queue_index,
      present_queue_index,
    };

    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.pQueueFamilyIndices = queue_families;
    swapchain_create_info.queueFamilyIndexCount = EG_ARRAY_COUNT(queue_families);
  }

  EG_VULKAN_CHECK(vkCreateSwapchainKHR(eg_context_device(), &swapchain_create_info, 0, &s_swapchain_current->handle));

  s_swapchain_current->color_image = (VkImage *)eg_heap_alloc(sizeof(VkImage) * s_swapchain_current->image_count, 0, 0);
  s_swapchain_current->color_image_view = (VkImageView *)eg_heap_alloc(sizeof(VkImageView) * s_swapchain_current->image_count, 0, 0);

  s_swapchain_current->depth_image = (VkImage *)eg_heap_alloc(sizeof(VkImage) * s_swapchain_current->image_count, 0, 0);
  s_swapchain_current->depth_device_memory = (VkDeviceMemory *)eg_heap_alloc(sizeof(VkDeviceMemory) * s_swapchain_current->image_count, 0, 0);
  s_swapchain_current->depth_image_view = (VkImageView *)eg_heap_alloc(sizeof(VkImageView) * s_swapchain_current->image_count, 0, 0);

  s_swapchain_current->frame_buffer = (VkFramebuffer *)eg_heap_alloc(sizeof(VkFramebuffer) * s_swapchain_current->image_count, 0, 0);

  eg_swapchain_create_color_images();
  eg_swapchain_create_depth_images();
  eg_swapchain_create_frame_buffer();
}
void eg_swapchain_destroy(void) {
  eg_swapchain_destroy_frame_buffer();
  eg_swapchain_destroy_depth_images();
  eg_swapchain_destroy_color_images();

  eg_heap_free(s_swapchain_current->color_image);
  eg_heap_free(s_swapchain_current->color_image_view);

  eg_heap_free(s_swapchain_current->depth_image);
  eg_heap_free(s_swapchain_current->depth_device_memory);
  eg_heap_free(s_swapchain_current->depth_image_view);

  eg_heap_free(s_swapchain_current->frame_buffer);

  vkDestroySwapchainKHR(eg_context_device(), s_swapchain_current->handle, 0);

  eg_database_destroy_swapchain_asset(&s_swapchain_current->asset);

  eg_heap_free(s_swapchain_current);
}

uint8_t eg_swapchain_is_dirty(void) {
  return s_swapchain_current->is_dirty;
}
uint32_t eg_swapchain_image_count(void) {
  return s_swapchain_current->image_count;
}
VkSwapchainKHR eg_swapchain_handle(void) {
  return s_swapchain_current->handle;
}
VkFramebuffer eg_swapchain_frame_buffer(uint32_t index) {
  return s_swapchain_current->frame_buffer[index];
}

void eg_swapchain_set_dirty(uint8_t is_dirty) {
  s_swapchain_current->is_dirty = is_dirty;
}

static void eg_swapchain_create_color_images(void) {
  EG_VULKAN_CHECK(vkGetSwapchainImagesKHR(eg_context_device(), s_swapchain_current->handle, (uint32_t *)&s_swapchain_current->image_count, s_swapchain_current->color_image));

  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = s_swapchain_current->color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = eg_context_surface_image_color_format();
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(eg_context_device(), &image_view_create_info, 0, &s_swapchain_current->color_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_depth_images(void) {
  uint32_t window_width = eg_context_window_width();
  uint32_t window_height = eg_context_window_height();

  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = window_width;
    image_create_info.extent.height = window_height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = eg_context_surface_image_depth_format();
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    EG_VULKAN_CHECK(vkCreateImage(eg_context_device(), &image_create_info, 0, &s_swapchain_current->depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(eg_context_device(), s_swapchain_current->depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = eg_context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    EG_VULKAN_CHECK(vkAllocateMemory(eg_context_device(), &memory_allocate_info, 0, &s_swapchain_current->depth_device_memory[image_index]));
    EG_VULKAN_CHECK(vkBindImageMemory(eg_context_device(), s_swapchain_current->depth_image[image_index], s_swapchain_current->depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = s_swapchain_current->depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = eg_context_surface_image_depth_format();
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(eg_context_device(), &image_view_create_info, 0, &s_swapchain_current->depth_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_frame_buffer(void) {
  uint32_t window_width = eg_context_window_width();
  uint32_t window_height = eg_context_window_height();

  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {
      s_swapchain_current->color_image_view[image_index],
      s_swapchain_current->depth_image_view[image_index],
    };

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = eg_renderpass_main();
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = EG_ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = window_width;
    frame_buffer_create_info.height = window_height;
    frame_buffer_create_info.layers = 1;

    EG_VULKAN_CHECK(vkCreateFramebuffer(eg_context_device(), &frame_buffer_create_info, 0, &s_swapchain_current->frame_buffer[image_index]));

    image_index++;
  }
}

static void eg_swapchain_destroy_color_images(void) {
  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(eg_context_device(), s_swapchain_current->color_image_view[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_depth_images(void) {
  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(eg_context_device(), s_swapchain_current->depth_image_view[image_index], 0);
    vkFreeMemory(eg_context_device(), s_swapchain_current->depth_device_memory[image_index], 0);
    vkDestroyImage(eg_context_device(), s_swapchain_current->depth_image[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_frame_buffer(void) {
  uint32_t image_index = 0;
  uint32_t image_count = s_swapchain_current->image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(eg_context_device(), s_swapchain_current->frame_buffer[image_index], 0);

    image_index++;
  }
}
