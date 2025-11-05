#include <editor/ed_pch.h>
#include <editor/ed_viewport.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_titlebar.h>

static char const *s_viewport_gbuffer_attachment_names[] = {"Color", "Depth"};

viewport_t *viewport_create(context_t *context, char const *name) {
  viewport_t *viewport = (viewport_t *)heap_alloc(sizeof(viewport_t), 1, 0);

  viewport->context = context;
  viewport->width = 1;
  viewport->height = 1;
  viewport->is_dirty = true;
  viewport->is_open = true;
  viewport->gbuffer_color_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * viewport->context->swapchain->image_count, 0, 0);
  viewport->gbuffer_depth_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * viewport->context->swapchain->image_count, 0, 0);

  strcpy(viewport->name, name);

  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    VkSampler gbuffer_color_sampler = viewport->context->swapchain->gbuffer_color_sampler[viewport->index][image_index];
    VkImageView gbuffer_color_image_view = viewport->context->swapchain->gbuffer_color_image_view[viewport->index][image_index];
    VkImageLayout gbuffer_color_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    viewport->gbuffer_color_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_color_sampler, gbuffer_color_image_view, gbuffer_color_image_layout);

    VkSampler gbuffer_depth_sampler = viewport->context->swapchain->gbuffer_depth_sampler[viewport->index][image_index];
    VkImageView gbuffer_depth_image_view = viewport->context->swapchain->gbuffer_depth_image_view[viewport->index][image_index];
    VkImageLayout gbuffer_depth_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    viewport->gbuffer_depth_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_depth_sampler, gbuffer_depth_image_view, gbuffer_depth_image_layout);

    image_index++;
  }

  return viewport;
}
void viewport_refresh(viewport_t *viewport) {
}
void viewport_draw(viewport_t *viewport) {
  bool is_docked = false;

  if (dockspace_begin_child(viewport->name, &viewport->is_open, &is_docked)) {

    if (ImGui::BeginCombo("Buffer", s_viewport_gbuffer_attachment_names[viewport->gbuffer_attachment_type])) {

      uint64_t image_index = 0;
      uint64_t image_count = ARRAY_COUNT(s_viewport_gbuffer_attachment_names);

      while (image_index < image_count) {

        bool is_selected = (viewport->gbuffer_attachment_type == image_index);

        if (ImGui::Selectable(s_viewport_gbuffer_attachment_names[image_index], is_selected)) {
          viewport->gbuffer_attachment_type = (gbuffer_attachment_type_t)image_index;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }

        image_index++;
      }

      ImGui::EndCombo();
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImVec2 window_size = ImGui::GetWindowSize();

    window_size.y -= 25.0F;

    viewport->width = (uint32_t)window_size.x;
    viewport->height = (uint32_t)window_size.y;

    if ((viewport->width != viewport->prev_width) || (viewport->height != viewport->prev_height)) {
      viewport->is_dirty = 1;
      viewport->prev_width = viewport->width;
      viewport->prev_height = viewport->height;
    } else {
      viewport->is_dirty = 0;
    }

    ImVec2 image_position_min = ImGui::GetCursorScreenPos();
    ImVec2 image_position_max = ImVec2(image_position_min.x + window_size.x, image_position_min.y + window_size.y);

    VkDescriptorSet gbuffer_image = 0;

    switch (viewport->gbuffer_attachment_type) {
      case GBUFFER_ATTACHMENT_TYPE_COLOR: {

        gbuffer_image = viewport->gbuffer_color_attachment[viewport->context->renderer->image_index];

        break;
      }

      case GBUFFER_ATTACHMENT_TYPE_DEPTH: {

        gbuffer_image = viewport->gbuffer_depth_attachment[viewport->context->renderer->image_index];

        break;
      }
    }

    draw_list->AddImageRounded(gbuffer_image, image_position_min, image_position_max, ImVec2(0.0F, 0.0F), ImVec2(1.0F, 1.0F), IM_COL32_WHITE, 5.0F);

    dockspace_end_child(is_docked);
  }
}
void viewport_destroy(viewport_t *viewport) {
  VULKAN_CHECK(vkQueueWaitIdle(viewport->context->graphic_queue));

  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    ImGui_ImplVulkan_RemoveTexture(viewport->gbuffer_color_attachment[image_index]);
    ImGui_ImplVulkan_RemoveTexture(viewport->gbuffer_depth_attachment[image_index]);

    image_index++;
  }

  heap_free(viewport->gbuffer_color_attachment);
  heap_free(viewport->gbuffer_depth_attachment);

  heap_free(viewport);
}
