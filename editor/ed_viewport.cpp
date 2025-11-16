#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_viewport.h>

static void ed_viewport_create_attachments(ed_viewport_t *viewport);
static void ed_viewport_destroy_attachments(ed_viewport_t *viewport);

ed_viewport_t ed_viewport_create(eg_context_t *context) {
  ed_viewport_t viewport = {0};

  viewport.context = context;
  viewport.width = 1;
  viewport.height = 1;
  viewport.prev_width = 1;
  viewport.prev_height = 1;
  viewport.is_dirty = 0;
  viewport.is_open = 1;
  viewport.is_docked = 0;
  viewport.handle = eg_viewport_create(context, 1, 1);
  viewport.gbuffer_color_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * context->swapchain->image_count, 0, 0);
  viewport.gbuffer_depth_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * context->swapchain->image_count, 0, 0);

  ed_viewport_create_attachments(&viewport);

  return viewport;
}
void ed_viewport_refresh(ed_viewport_t *viewport) {
  if (viewport->is_dirty) {

    viewport->is_dirty = 0;

    ed_viewport_destroy_attachments(viewport);

    eg_viewport_resize(viewport->handle, viewport->width, viewport->height);

    ed_viewport_create_attachments(viewport);
  }
}
void ed_viewport_draw(ed_viewport_t *viewport, uint8_t enable_controls) {
  if (enable_controls) {
    ImGui::SetNextItemWidth(200.0F);

    if (ImGui::BeginCombo("Attachment", g_viewport_gbuffer_attachment_names[viewport->gbuffer_attachment_type])) {

      uint64_t attachment_index = 0;
      uint64_t attachment_count = ED_GBUFFER_ATTACHMENT_TYPE_COUNT;

      while (attachment_index < attachment_count) {

        uint8_t is_selected = (viewport->gbuffer_attachment_type == attachment_index);

        if (ImGui::Selectable(g_viewport_gbuffer_attachment_names[attachment_index], is_selected)) {
          viewport->gbuffer_attachment_type = (ed_gbuffer_attachment_type_t)attachment_index;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }

        attachment_index++;
      }

      ImGui::EndCombo();
    }
  }

  ImVec2 window_size = ImGui::GetWindowSize();

  if (enable_controls) {
    window_size.y -= 25.0F;
  }

  viewport->width = window_size.x < 0 ? 1 : (uint32_t)window_size.x;
  viewport->height = window_size.y < 0 ? 1 : (uint32_t)window_size.y;
  viewport->is_dirty = ((viewport->width != viewport->prev_width) || (viewport->height != viewport->prev_height));

  if (viewport->is_dirty) {
    viewport->prev_width = viewport->width;
    viewport->prev_height = viewport->height;
    viewport->is_dirty = 1;
  }

  ImVec2 image_position_min = ImGui::GetCursorScreenPos();
  ImVec2 image_position_max = ImVec2(image_position_min.x + window_size.x, image_position_min.y + window_size.y);

  VkDescriptorSet gbuffer_image = 0;

  switch (viewport->gbuffer_attachment_type) {
    case ED_GBUFFER_ATTACHMENT_TYPE_COLOR: {

      gbuffer_image = viewport->gbuffer_color_attachment[viewport->context->renderer->image_index];

      break;
    }

    case ED_GBUFFER_ATTACHMENT_TYPE_DEPTH: {

      gbuffer_image = viewport->gbuffer_depth_attachment[viewport->context->renderer->image_index];

      break;
    }
  }

  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  draw_list->AddImageRounded(gbuffer_image, image_position_min, image_position_max, ImVec2(0.0F, 0.0F), ImVec2(1.0F, 1.0F), IM_COL32_WHITE, 1.0F);
}
void ed_viewport_destroy(ed_viewport_t *viewport) {
  ed_viewport_destroy_attachments(viewport);

  eg_viewport_destroy(viewport->handle);

  heap_free(viewport->gbuffer_color_attachment);
  heap_free(viewport->gbuffer_depth_attachment);
}

static void ed_viewport_create_attachments(ed_viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    VkSampler gbuffer_color_sampler = viewport->handle->color_sampler[image_index];
    VkImageView gbuffer_color_image_view = viewport->handle->color_image_view[image_index];
    VkImageLayout gbuffer_color_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    viewport->gbuffer_color_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_color_sampler, gbuffer_color_image_view, gbuffer_color_image_layout);

    VkSampler gbuffer_depth_sampler = viewport->handle->depth_sampler[image_index];
    VkImageView gbuffer_depth_image_view = viewport->handle->depth_image_view[image_index];
    VkImageLayout gbuffer_depth_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    viewport->gbuffer_depth_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_depth_sampler, gbuffer_depth_image_view, gbuffer_depth_image_layout);

    image_index++;
  }
}
static void ed_viewport_destroy_attachments(ed_viewport_t *viewport) {
  uint64_t image_index = 0;
  uint64_t image_count = viewport->context->swapchain->image_count;

  while (image_index < image_count) {

    ImGui_ImplVulkan_RemoveTexture(viewport->gbuffer_color_attachment[image_index]);
    ImGui_ImplVulkan_RemoveTexture(viewport->gbuffer_depth_attachment[image_index]);

    image_index++;
  }
}
