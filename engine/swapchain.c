#include <library/core/api.h>

#include <engine/context.h>
#include <engine/macros.h>
#include <engine/swapchain.h>

static void swapchain_compute_local_variables(void);

static void swapchain_create_render_pass(void);
static void swapchain_create_color_images(void);
static void swapchain_create_depth_images(void);
static void swapchain_create_frame_buffer(void);

static void swapchain_destroy_render_pass(void);
static void swapchain_destroy_color_images(void);
static void swapchain_destroy_depth_images(void);
static void swapchain_destroy_frame_buffer(void);

uint8_t g_swapchain_is_dirty = 0;

VkSwapchainKHR g_swapchain = 0;

int32_t g_swapchain_image_count = 0;

VkRenderPass g_swapchain_render_pass = 0;

VkFramebuffer *g_swapchain_frame_buffers = 0;

static int32_t s_swapchain_image_count = 0;

static VkFormat s_swapchain_depth_format = 0;

static VkImage *s_swapchain_color_image = 0;
static VkImage *s_swapchain_depth_image = 0;

static VkDeviceMemory *s_swapchain_depth_image_device_memory = 0;

static VkImageView *s_swapchain_color_image_view = 0;
static VkImageView *s_swapchain_depth_image_view = 0;

void swapchain_create(void) {
  swapchain_compute_local_variables();

  VkSwapchainCreateInfoKHR swapchain_create_info = {0};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.surface = g_context_surface;
  swapchain_create_info.minImageCount = s_swapchain_image_count;
  swapchain_create_info.imageFormat = g_context_prefered_surface_format.format;
  swapchain_create_info.imageColorSpace = g_context_prefered_surface_format.colorSpace;
  swapchain_create_info.imageExtent.width = g_context_surface_width;
  swapchain_create_info.imageExtent.height = g_context_surface_height;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_create_info.preTransform = g_context_surface_capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = g_context_prefered_present_mode;
  swapchain_create_info.clipped = 1;
  swapchain_create_info.oldSwapchain = 0;

  int32_t queue_families[2] = {g_context_graphics_queue_index, g_context_present_queue_index};

  if (g_context_graphics_queue_index == g_context_present_queue_index) {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = 0;
  } else {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.pQueueFamilyIndices = queue_families;
    swapchain_create_info.queueFamilyIndexCount = ARRAY_COUNT(queue_families);
  }

  VULKAN_CHECK(vkCreateSwapchainKHR(g_context_device, &swapchain_create_info, 0, &g_swapchain));

  swapchain_create_render_pass();
  swapchain_create_color_images();
  swapchain_create_depth_images();
  swapchain_create_frame_buffer();
}
void swapchain_destroy(void) {
  swapchain_destroy_frame_buffer();
  swapchain_destroy_depth_images();
  swapchain_destroy_color_images();
  swapchain_destroy_render_pass();

  vkDestroySwapchainKHR(g_context_device, g_swapchain, 0);
}

static void swapchain_compute_local_variables(void) {
  s_swapchain_image_count = g_swapchain_image_count;
  s_swapchain_image_count = MAX(s_swapchain_image_count, (int32_t)g_context_surface_capabilities.minImageCount);
  s_swapchain_image_count = MIN(s_swapchain_image_count, (int32_t)g_context_surface_capabilities.maxImageCount);
  g_swapchain_image_count = s_swapchain_image_count;

  s_swapchain_depth_format = VK_FORMAT_D32_SFLOAT;
}

static void swapchain_create_render_pass(void) {
  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = g_context_prefered_surface_format.format;
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = s_swapchain_depth_format;
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

  VULKAN_CHECK(vkCreateRenderPass(g_context_device, &render_pass_create_info, 0, &g_swapchain_render_pass));
}
static void swapchain_create_color_images(void) {
  s_swapchain_color_image = (VkImage *)heap_alloc(sizeof(VkImage) * s_swapchain_image_count);
  s_swapchain_color_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * s_swapchain_image_count);

  VULKAN_CHECK(vkGetSwapchainImagesKHR(g_context_device, g_swapchain, &s_swapchain_image_count, s_swapchain_color_image));

  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = s_swapchain_color_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = g_context_prefered_surface_format.format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(g_context_device, &image_view_create_info, 0, &s_swapchain_color_image_view[image_index]));

    image_index++;
  }
}
static void swapchain_create_depth_images(void) {
  s_swapchain_depth_image = (VkImage *)heap_alloc(sizeof(VkImage) * s_swapchain_image_count);
  s_swapchain_depth_image_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_swapchain_image_count);
  s_swapchain_depth_image_view = (VkImageView *)heap_alloc(sizeof(VkImageView) * s_swapchain_image_count);

  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = g_context_surface_width;
    image_create_info.extent.height = g_context_surface_height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.format = s_swapchain_depth_format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateImage(g_context_device, &image_create_info, 0, &s_swapchain_depth_image[image_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetImageMemoryRequirements(g_context_device, s_swapchain_depth_image[image_index], &memory_requirements);

    uint32_t memory_type_index = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_swapchain_depth_image_device_memory[image_index]));
    VULKAN_CHECK(vkBindImageMemory(g_context_device, s_swapchain_depth_image[image_index], s_swapchain_depth_image_device_memory[image_index], 0));

    VkImageViewCreateInfo image_view_create_info = {0};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.image = s_swapchain_depth_image[image_index];
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = s_swapchain_depth_format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    VULKAN_CHECK(vkCreateImageView(g_context_device, &image_view_create_info, 0, &s_swapchain_depth_image_view[image_index]));

    image_index++;
  }
}
static void swapchain_create_frame_buffer(void) {
  g_swapchain_frame_buffers = (VkFramebuffer *)heap_alloc(sizeof(VkFramebuffer) * s_swapchain_image_count);

  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    VkImageView image_attachments[] = {s_swapchain_color_image_view[image_index], s_swapchain_depth_image_view[image_index]};

    VkFramebufferCreateInfo frame_buffer_create_info = {0};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = g_swapchain_render_pass;
    frame_buffer_create_info.pAttachments = image_attachments;
    frame_buffer_create_info.attachmentCount = ARRAY_COUNT(image_attachments);
    frame_buffer_create_info.width = g_context_surface_width;
    frame_buffer_create_info.height = g_context_surface_height;
    frame_buffer_create_info.layers = 1;

    VULKAN_CHECK(vkCreateFramebuffer(g_context_device, &frame_buffer_create_info, 0, &g_swapchain_frame_buffers[image_index]));

    image_index++;
  }
}

static void swapchain_destroy_render_pass(void) {
  vkDestroyRenderPass(g_context_device, g_swapchain_render_pass, 0);
}
static void swapchain_destroy_color_images(void) {
  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    vkDestroyImageView(g_context_device, s_swapchain_color_image_view[image_index], 0);

    image_index++;
  }

  heap_free(s_swapchain_color_image);
  heap_free(s_swapchain_color_image_view);
}
static void swapchain_destroy_depth_images(void) {
  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    vkDestroyImageView(g_context_device, s_swapchain_depth_image_view[image_index], 0);
    vkFreeMemory(g_context_device, s_swapchain_depth_image_device_memory[image_index], 0);
    vkDestroyImage(g_context_device, s_swapchain_depth_image[image_index], 0);

    image_index++;
  }

  heap_free(s_swapchain_depth_image);
  heap_free(s_swapchain_depth_image_device_memory);
  heap_free(s_swapchain_depth_image_view);
}
static void swapchain_destroy_frame_buffer(void) {
  int32_t image_index = 0;
  while (image_index < s_swapchain_image_count) {
    vkDestroyFramebuffer(g_context_device, g_swapchain_frame_buffers[image_index], 0);

    image_index++;
  }

  heap_free(g_swapchain_frame_buffers);
}
