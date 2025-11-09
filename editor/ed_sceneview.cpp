#include <editor/ed_pch.h>
#include <editor/ed_sceneview.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_main.h>

#include <imgui/imgui_impl_vulkan.h>

static char const *s_sceneview_gbuffer_attachment_names[] = {"Color", "Depth"};

void sceneview_create(context_t *context, uint64_t link_index, uint32_t width, uint32_t height, char const *name) {
  sceneview_t *sceneview = (sceneview_t *)heap_alloc(sizeof(sceneview_t), 1, 0);

  sceneview->context = context;
  sceneview->link_index = link_index;
  sceneview->width = width;
  sceneview->height = height;
  sceneview->prev_width = width;
  sceneview->prev_height = height;
  sceneview->is_dirty = 0;
  sceneview->is_open = 1;
  sceneview->is_docked = 0;
  sceneview->viewport = context_viewport_create(sceneview->context, sceneview->link_index, sceneview->width, sceneview->height);
  sceneview->gbuffer_color_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * sceneview->context->swapchain->image_count, 0, 0);
  sceneview->gbuffer_depth_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * sceneview->context->swapchain->image_count, 0, 0);

  strcpy(sceneview->name, name);

  uint64_t image_index = 0;
  uint64_t image_count = sceneview->context->swapchain->image_count;

  while (image_index < image_count) {

    VkSampler gbuffer_color_sampler = sceneview->viewport->color_sampler[image_index];
    VkImageView gbuffer_color_image_view = sceneview->viewport->color_image_view[image_index];
    VkImageLayout gbuffer_color_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    sceneview->gbuffer_color_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_color_sampler, gbuffer_color_image_view, gbuffer_color_image_layout);

    VkSampler gbuffer_depth_sampler = sceneview->viewport->depth_sampler[image_index];
    VkImageView gbuffer_depth_image_view = sceneview->viewport->depth_image_view[image_index];
    VkImageLayout gbuffer_depth_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    sceneview->gbuffer_depth_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_depth_sampler, gbuffer_depth_image_view, gbuffer_depth_image_layout);

    image_index++;
  }

  g_sceneviews[sceneview->link_index] = sceneview;
}
void sceneview_refresh(sceneview_t *sceneview) {
}
void sceneview_draw(sceneview_t *sceneview) {
  if (dockspace_begin_child(sceneview->name, &sceneview->is_open, &sceneview->is_docked)) {

    ImGui::SetNextItemWidth(200.0F);

    if (ImGui::BeginCombo("Attachment", s_sceneview_gbuffer_attachment_names[sceneview->gbuffer_attachment_type])) {

      uint64_t attachment_index = 0;
      uint64_t attachment_count = ARRAY_COUNT(s_sceneview_gbuffer_attachment_names);

      while (attachment_index < attachment_count) {

        uint8_t is_selected = (sceneview->gbuffer_attachment_type == attachment_index);

        if (ImGui::Selectable(s_sceneview_gbuffer_attachment_names[attachment_index], is_selected)) {
          sceneview->gbuffer_attachment_type = (gbuffer_attachment_type_t)attachment_index;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }

        attachment_index++;
      }

      ImGui::EndCombo();
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImVec2 window_size = ImGui::GetWindowSize();

    window_size.y -= 25.0F;

    sceneview->width = (uint32_t)window_size.x;
    sceneview->height = (uint32_t)window_size.y;
    sceneview->is_dirty = ((sceneview->width != sceneview->prev_width) || (sceneview->height != sceneview->prev_height));

    if (sceneview->is_dirty) {
      sceneview->prev_width = sceneview->width;
      sceneview->prev_height = sceneview->height;
      sceneview->is_dirty = 1;
    }

    ImVec2 image_position_min = ImGui::GetCursorScreenPos();
    ImVec2 image_position_max = ImVec2(image_position_min.x + window_size.x, image_position_min.y + window_size.y);

    VkDescriptorSet gbuffer_image = 0;

    switch (sceneview->gbuffer_attachment_type) {
      case GBUFFER_ATTACHMENT_TYPE_COLOR: {

        gbuffer_image = sceneview->gbuffer_color_attachment[sceneview->context->renderer->image_index];

        break;
      }

      case GBUFFER_ATTACHMENT_TYPE_DEPTH: {

        gbuffer_image = sceneview->gbuffer_depth_attachment[sceneview->context->renderer->image_index];

        break;
      }
    }

    draw_list->AddImageRounded(gbuffer_image, image_position_min, image_position_max, ImVec2(0.0F, 0.0F), ImVec2(1.0F, 1.0F), IM_COL32_WHITE, 2.5F);

    dockspace_end_child(sceneview->is_docked);
  }
}
void sceneview_destroy(sceneview_t *sceneview) {
  g_sceneviews[sceneview->link_index] = 0;

  uint64_t image_index = 0;
  uint64_t image_count = sceneview->context->swapchain->image_count;

  while (image_index < image_count) {

    ImGui_ImplVulkan_RemoveTexture(sceneview->gbuffer_color_attachment[image_index]);
    ImGui_ImplVulkan_RemoveTexture(sceneview->gbuffer_depth_attachment[image_index]);

    image_index++;
  }

  context_viewport_destroy(sceneview->context, sceneview->link_index);

  heap_free(sceneview->gbuffer_color_attachment);
  heap_free(sceneview->gbuffer_depth_attachment);

  heap_free(sceneview);
}
