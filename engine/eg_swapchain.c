#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_swapchain.h>

static void eg_swapchain_create_main_render_pass(eg_swapchain_t *swapchain);
static void eg_swapchain_create_color_images(eg_swapchain_t *swapchain);
static void eg_swapchain_create_depth_images(eg_swapchain_t *swapchain);
static void eg_swapchain_create_frame_buffer(eg_swapchain_t *swapchain);

static void eg_swapchain_destroy_main_render_pass(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_color_images(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_depth_images(eg_swapchain_t *swapchain);
static void eg_swapchain_destroy_frame_buffer(eg_swapchain_t *swapchain);

void eg_swapchain_create(eg_context_t *context) {
  eg_swapchain_t *swapchain = (eg_swapchain_t *)heap_alloc(sizeof(eg_swapchain_t), 1, 0);

  swapchain->context = context;
  swapchain->context->swapchain = swapchain;

  swapchain_asset_t swapchain_asset = database_load_swapchain_default_asset();

  swapchain->image_count = swapchain_asset.image_count;
  swapchain->depth_format = swapchain_asset.depth_format;

  swapchain->image_count = MAX(swapchain->image_count, swapchain->context->surface_capabilities.minImageCount);
  swapchain->image_count = MIN(swapchain->image_count, swapchain->context->surface_capabilities.maxImageCount);

  swapchain->min_image_count = swapchain->context->surface_capabilities.minImageCount;

  VkSwapchainCreateInfoKHR swapchain_create_info = {0};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.surface = swapchain->context->surface;
  swapchain_create_info.minImageCount = (uint32_t)swapchain->image_count;
  swapchain_create_info.imageFormat = swapchain->context->prefered_surface_format.format;
  swapchain_create_info.imageColorSpace = swapchain->context->prefered_surface_format.colorSpace;
  swapchain_create_info.imageExtent.width = swapchain->context->window_width;
  swapchain_create_info.imageExtent.height = swapchain->context->window_height;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_create_info.preTransform = swapchain->context->surface_capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = swapchain->context->prefered_present_mode;
  swapchain_create_info.clipped = 1;
  swapchain_create_info.oldSwapchain = 0;

  int32_t queue_families[2] = {swapchain->context->graphic_queue_index, swapchain->context->present_queue_index};

  if (swapchain->context->graphic_queue_index == swapchain->context->present_queue_index) {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = 0;
  } else {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.pQueueFamilyIndices = queue_families;
    swapchain_create_info.queueFamilyIndexCount = ARRAY_COUNT(queue_families);
  }

  VULKAN_CHECK(vkCreateSwapchainKHR(swapchain->context->device, &swapchain_create_info, 0, &swapchain->handle));

  swapchain->color_image = (VkImage *)heap_alloc(sizeof(VkImage) * swapchain->image_count, 0, 0);
  swapchain->color_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * swapchain->image_count, 0, 0);

  swapchain->depth_image = (VkImage *)heap_alloc(sizeof(VkImage) * swapchain->image_count, 0, 0);
  swapchain->depth_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * swapchain->image_count, 0, 0);
  swapchain->depth_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * swapchain->image_count, 0, 0);

  swapchain->frame_buffer = (VkFramebuffer *)heap_alloc(sizeof(VkFramebuffer) * swapchain->image_count, 0, 0);

  eg_swapchain_create_main_render_pass(swapchain);
  eg_swapchain_create_color_images(swapchain);
  eg_swapchain_create_depth_images(swapchain);
  eg_swapchain_create_frame_buffer(swapchain);

  database_destroy_swapchain_asset(&swapchain_asset);
}
void eg_swapchain_destroy(eg_swapchain_t *swapchain) {
  eg_swapchain_destroy_frame_buffer(swapchain);
  eg_swapchain_destroy_depth_images(swapchain);
  eg_swapchain_destroy_color_images(swapchain);
  eg_swapchain_destroy_main_render_pass(swapchain);

  heap_free(swapchain->color_image);
  heap_free(swapchain->color_image_view);

  heap_free(swapchain->depth_image);
  heap_free(swapchain->depth_device_memory);
  heap_free(swapchain->depth_image_view);

  heap_free(swapchain->frame_buffer);

  vkDestroySwapchainKHR(swapchain->context->device, swapchain->handle, 0);

  swapchain->context->swapchain = 0;

  heap_free(swapchain);
}

static void eg_swapchain_create_main_render_pass(eg_swapchain_t *swapchain) {
  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = swapchain->context->prefered_surface_format.format;
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = swapchain->depth_format;
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
  render_pass_create_info.attachmentCount = ARRAY_COUNT(attachment_descriptions);
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;
  render_pass_create_info.dependencyCount = 1;

  VULKAN_CHECK(vkCreateRenderPass(swapchain->context->device, &render_pass_create_info, 0, &swapchain->main_render_pass));
}
static void eg_swapchain_create_color_images(eg_swapchain_t *swapchain) {
  VULKAN_CHECK(vkGetSwapchainImagesKHR(swapchain->context->device, swapchain->handle, (uint32_t *)&swapchain->image_count, swapchain->color_image));

  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = swapchain->color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = swapchain->context->prefered_surface_format.format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(swapchain->context->device, &image_view_create_info, 0, &swapchain->color_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_depth_images(eg_swapchain_t *swapchain) {
  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = swapchain->context->window_width;
    image_create_info.extent.height = swapchain->context->window_height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = swapchain->depth_format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateImage(swapchain->context->device, &image_create_info, 0, &swapchain->depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(swapchain->context->device, swapchain->depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = eg_context_find_memory_type(swapchain->context, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    VULKAN_CHECK(vkAllocateMemory(swapchain->context->device, &memory_allocate_info, 0, &swapchain->depth_device_memory[image_index]));
    VULKAN_CHECK(vkBindImageMemory(swapchain->context->device, swapchain->depth_image[image_index], swapchain->depth_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = swapchain->depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = swapchain->depth_format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(swapchain->context->device, &image_view_create_info, 0, &swapchain->depth_image_view[image_index]));

    image_index++;
  }
}
static void eg_swapchain_create_frame_buffer(eg_swapchain_t *swapchain) {
  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    VkImageView image_attachments[] = {swapchain->color_image_view[image_index], swapchain->depth_image_view[image_index]};

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = swapchain->main_render_pass;
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = swapchain->context->window_width;
    frame_buffer_create_info.height = swapchain->context->window_height;
    frame_buffer_create_info.layers = 1;

    VULKAN_CHECK(vkCreateFramebuffer(swapchain->context->device, &frame_buffer_create_info, 0, &swapchain->frame_buffer[image_index]));

    image_index++;
  }
}

static void eg_swapchain_destroy_main_render_pass(eg_swapchain_t *swapchain) {
  vkDestroyRenderPass(swapchain->context->device, swapchain->main_render_pass, 0);
}
static void eg_swapchain_destroy_color_images(eg_swapchain_t *swapchain) {
  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(swapchain->context->device, swapchain->color_image_view[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_depth_images(eg_swapchain_t *swapchain) {
  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyImageView(swapchain->context->device, swapchain->depth_image_view[image_index], 0);
    vkFreeMemory(swapchain->context->device, swapchain->depth_device_memory[image_index], 0);
    vkDestroyImage(swapchain->context->device, swapchain->depth_image[image_index], 0);

    image_index++;
  }
}
static void eg_swapchain_destroy_frame_buffer(eg_swapchain_t *swapchain) {
  uint64_t image_index = 0;
  uint64_t image_count = swapchain->image_count;

  while (image_index < image_count) {

    vkDestroyFramebuffer(swapchain->context->device, swapchain->frame_buffer[image_index], 0);

    image_index++;
  }
}
