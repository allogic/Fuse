#include <engine/eg_pch.h>

static VkRenderPass s_renderpass_main = 0;
static VkRenderPass s_renderpass_gbuffer = 0;

VkRenderPass eg_renderpass_main(void) {
  return s_renderpass_main;
}
VkRenderPass eg_renderpass_gbuffer(void) {
  return s_renderpass_gbuffer;
}

void eg_renderpass_create_main(void) {
  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = eg_context_surface_image_color_format();
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = eg_context_surface_image_depth_format();
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
  render_pass_create_info.attachmentCount = EG_ARRAY_COUNT(attachment_descriptions);
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;
  render_pass_create_info.dependencyCount = 1;

  EG_VULKAN_CHECK(vkCreateRenderPass(eg_context_device(), &render_pass_create_info, 0, &s_renderpass_main));
}
void eg_renderpass_create_gbuffer(void) {
  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = eg_context_surface_image_color_format();
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = eg_context_surface_image_depth_format();
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

  VkAttachmentDescription attachment_descriptions[] = {
    color_attachment_description,
    depth_attachment_description,
  };

  VkRenderPassCreateInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.pAttachments = attachment_descriptions;
  render_pass_create_info.attachmentCount = EG_ARRAY_COUNT(attachment_descriptions);
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;
  render_pass_create_info.dependencyCount = 1;

  EG_VULKAN_CHECK(vkCreateRenderPass(eg_context_device(), &render_pass_create_info, 0, &s_renderpass_gbuffer));
}

void eg_renderpass_destroy_main(void) {
  vkDestroyRenderPass(eg_context_device(), s_renderpass_main, 0);
}
void eg_renderpass_destroy_gbuffer(void) {
  vkDestroyRenderPass(eg_context_device(), s_renderpass_gbuffer, 0);
}
