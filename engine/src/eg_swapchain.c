#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_swapchain.h>

struct eg_swapchain_t {
  eg_context_t *context;
  lb_swapchain_asset_t asset;
  uint8_t is_dirty;
  uint32_t min_image_count;
  uint32_t image_count;
  VkSwapchainKHR handle;
  VkRenderPass main_render_pass;
  VkImage *color_image;
  VkImageView *color_image_view;
  VkImage *depth_image;
  VkDeviceMemory *depth_device_memory;
  VkImageView *depth_image_view;
  VkFramebuffer *frame_buffer;
};

static void eg_swapchain_create_main_render_pass(eg_swapchain_t *swapchain);
static void eg_swapchain_create_color_images(eg_swapchain_t *swapchain);
static void eg_swapchain_create_depth_images(eg_swapchain_t *swapchain);
static void eg_swapchain_create_frame_buffer(eg_swapchain_t *swapchain);

static void eg_swapchain_destroy_main_render_pass(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_color_images(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_depth_images(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_frame_buffer(eg_swapchain_t *swapchain);

void eg_swapchain_create(eg_context_t *context, lb_swapchain_asset_id_t swapchain_asset_id) {
  eg_swapchain_t *swapchain = (eg_swapchain_t *)lb_heap_alloc(sizeof(eg_swapchain_t), 1, 0);

  eg_context_set_swapchain(context, swapchain);

  swapchain->context = context;
  swapchain->asset = lb_database_load_swapchain_asset_by_id(swapchain_asset_id);
  swapchain->image_count = swapchain->asset.image_count;
  swapchain->image_count = LB_MAX(swapchain->image_count, swapchain->min_image_count);
  swapchain->image_count = LB_MIN(swapchain->image_count, swapchain->min_image_count);
  swapchain->min_image_count = eg_context_surface_min_image_count(swapchain->context);

  VkDevice device = eg_context_device(swapchain->context);
  VkSurfaceKHR surface = eg_context_surface(swapchain->context);
  VkSurfaceTransformFlagBitsKHR surface_transform = eg_context_surface_transform(swapchain->context);
  VkPresentModeKHR surface_present_mode = eg_context_surface_present_mode(swapchain->context);
  VkFormat surface_image_color_format = eg_context_surface_image_color_format(swapchain->context);
  VkColorSpaceKHR surface_image_color_space = eg_context_surface_image_color_space(swapchain->context);
  uint32_t window_width = eg_context_window_width(swapchain->context);
  uint32_t window_height = eg_context_window_height(swapchain->context);

  VkSwapchainCreateInfoKHR swapchain_create_info = {0};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.surface = surface;
  swapchain_create_info.minImageCount = (uint32_t)swapchain->image_count;
  swapchain_create_info.imageFormat = surface_image_color_format;
  swapchain_create_info.imageColorSpace = surface_image_color_space;
  swapchain_create_info.imageExtent.width = window_width;
  swapchain_create_info.imageExtent.height = window_height;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_create_info.preTransform = surface_transform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = surface_present_mode;
  swapchain_create_info.clipped = 1;
  swapchain_create_info.oldSwapchain = 0;

  int32_t primary_queue_index = eg_context_primary_queue_index(swapchain->context);
  int32_t present_queue_index = eg_context_present_queue_index(swapchain->context);

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
    swapchain_create_info.queueFamilyIndexCount = LB_ARRAY_COUNT(queue_families);
  }

  EG_VULKAN_CHECK(vkCreateSwapchainKHR(device, &swapchain_create_info, 0, &swapchain->handle));

  swapchain->color_image = (VkImage *)lb_heap_alloc(sizeof(VkImage) * swapchain->image_count, 0, 0);
  swapchain->color_image_view = (VkImageView *)lb_heap_alloc(sizeof(VkImageView) * swapchain->image_count, 0, 0);

  swapchain->depth_image = (VkImage *)lb_heap_alloc(sizeof(VkImage) * swapchain->image_count, 0, 0);
  swapchain->depth_device_memory = (VkDeviceMemory *)lb_heap_alloc(sizeof(VkDeviceMemory) * swapchain->image_count, 0, 0);
  swapchain->depth_image_view = (VkImageView *)lb_heap_alloc(sizeof(VkImageView) * swapchain->image_count, 0, 0);

  swapchain->frame_buffer = (VkFramebuffer *)lb_heap_alloc(sizeof(VkFramebuffer) * swapchain->image_count, 0, 0);

  eg_swapchain_create_main_render_pass(swapchain);
  eg_swapchain_create_color_images(swapchain);
  eg_swapchain_create_depth_images(swapchain);
  eg_swapchain_create_frame_buffer(swapchain);
}
void eg_swapchain_destroy(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);

  eg_swapchain_destroy_frame_buffer(swapchain);
  eg_swapchain_destroy_depth_images(swapchain);
  eg_swapchain_destroy_color_images(swapchain);
  eg_swapchain_destroy_main_render_pass(swapchain);

  lb_heap_free(swapchain->color_image);
  lb_heap_free(swapchain->color_image_view);

  lb_heap_free(swapchain->depth_image);
  lb_heap_free(swapchain->depth_device_memory);
  lb_heap_free(swapchain->depth_image_view);

  lb_heap_free(swapchain->frame_buffer);

  vkDestroySwapchainKHR(device, swapchain->handle, 0);

  eg_context_set_swapchain(swapchain->context, 0);

  lb_database_destroy_swapchain_asset(&swapchain->asset);

  lb_heap_free(swapchain);
}

uint8_t eg_swapchain_is_dirty(eg_swapchain_t *swapchain) {
  return swapchain->is_dirty;
}
uint32_t eg_swapchain_min_image_count(eg_swapchain_t *swapchain) {
  return swapchain->min_image_count;
}
uint32_t eg_swapchain_image_count(eg_swapchain_t *swapchain) {
  return swapchain->image_count;
}
VkSwapchainKHR eg_swapchain_handle(eg_swapchain_t *swapchain) {
  return swapchain->handle;
}

void eg_swapchain_set_dirty(eg_swapchain_t *swapchain, uint8_t is_dirty) {
  swapchain->is_dirty = is_dirty;
}

static void eg_swapchain_create_main_render_pass(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);
  VkFormat surface_image_color_format = eg_context_surface_image_color_format(swapchain->context);
  VkFormat surface_image_depth_format = eg_context_surface_image_depth_format(swapchain->context);

  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = surface_image_color_format;
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = surface_image_depth_format;
  depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_reference = {0};
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_reference = {0};
  depth_attachment_reference.attachment = 1;
  depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = {0};
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;
  subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

  VkSubpassDependency subpass_dependency = {0};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.srcAccessMask = VK_ACCESS_NONE;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachment_descriptions[] = {color_attachment_description, depth_attachment_description};

  VkRenderPassCreateInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.pAttachments = attachment_descriptions;
  render_pass_create_info.attachmentCount = LB_ARRAY_COUNT(attachment_descriptions);
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;
  render_pass_create_info.dependencyCount = 1;

  EG_VULKAN_CHECK(vkCreateRenderPass(device, &render_pass_create_info, 0, &swapchain->main_render_pass));
}
static void eg_swapchain_create_color_images(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);
  VkFormat image_format = eg_context_surface_image_format(swapchain->context);

  EG_VULKAN_CHECK(vkGetSwapchainImagesKHR(device, swapchain->handle, (uint32_t *)&swapchain->image_count, swapchain->color_image));

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = swapchain->color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = image_format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(device, &image_view_create_info, 0, &swapchain->color_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_depth_images(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);
  VkFormat surface_image_depth_format = eg_context_surface_image_depth_format(swapchain->context);
  uint32_t window_width = eg_context_window_width(swapchain->context);
  uint32_t window_height = eg_context_window_height(swapchain->context);

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = window_width;
    image_create_info.extent.height = window_height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = surface_image_depth_format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    EG_VULKAN_CHECK(vkCreateImage(device, &image_create_info, 0, &swapchain->depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(device, swapchain->depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = eg_context_find_memory_type(swapchain->context, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    EG_VULKAN_CHECK(vkAllocateMemory(device, &memory_allocate_info, 0, &swapchain->depth_device_memory[image_index]));
    EG_VULKAN_CHECK(vkBindImageMemory(device, swapchain->depth_image[image_index], swapchain->depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = swapchain->depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = surface_image_depth_format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    EG_VULKAN_CHECK(vkCreateImageView(device, &image_view_create_info, 0, &swapchain->depth_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_frame_buffer(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);
  uint32_t window_width = eg_context_window_width(swapchain->context);
  uint32_t window_height = eg_context_window_height(swapchain->context);

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {swapchain->color_image_view[image_index], swapchain->depth_image_view[image_index]};

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = swapchain->main_render_pass;
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = LB_ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = window_width;
    frame_buffer_create_info.height = window_height;
    frame_buffer_create_info.layers = 1;

    EG_VULKAN_CHECK(vkCreateFramebuffer(device, &frame_buffer_create_info, 0, &swapchain->frame_buffer[image_index]));

    image_index++;
  }
}

static void eg_swapchain_destroy_main_render_pass(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);

  vkDestroyRenderPass(device, swapchain->main_render_pass, 0);
}
static void eg_swapchain_destroy_color_images(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(device, swapchain->color_image_view[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_depth_images(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(device, swapchain->depth_image_view[image_index], 0);
    vkFreeMemory(device, swapchain->depth_device_memory[image_index], 0);
    vkDestroyImage(device, swapchain->depth_image[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_frame_buffer(eg_swapchain_t *swapchain) {
  VkDevice device = eg_context_device(swapchain->context);

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(device, swapchain->frame_buffer[image_index], 0);

    image_index++;
  }
}
