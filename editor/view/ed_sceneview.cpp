#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_dockspace.h>

#include <editor/view/ed_sceneview.h>

static void sceneview_create_attachments(context_t *context);
static void sceneview_destroy_attachments(context_t *context);

static uint32_t s_sceneview_width = 1;
static uint32_t s_sceneview_height = 1;
static uint32_t s_sceneview_prev_width = 1;
static uint32_t s_sceneview_prev_height = 1;

static uint8_t s_sceneview_is_dirty = 0;
static uint8_t s_sceneview_is_open = 1;
static uint8_t s_sceneview_is_docked = 0;

static viewport_t *s_sceneview_viewport = 0;

static VkDescriptorSet *s_sceneview_gbuffer_color_attachment = 0;
static VkDescriptorSet *s_sceneview_gbuffer_depth_attachment = 0;

static gbuffer_attachment_type_t s_sceneview_gbuffer_attachment_type = GBUFFER_ATTACHMENT_TYPE_COLOR;

static char const *s_sceneview_gbuffer_attachment_names[] = {"Color", "Depth"};

void sceneview_create(context_t *context) {
  s_sceneview_width = 1;
  s_sceneview_height = 1;
  s_sceneview_prev_width = 1;
  s_sceneview_prev_height = 1;

  s_sceneview_is_dirty = 0;
  s_sceneview_is_open = 1;
  s_sceneview_is_docked = 0;

  s_sceneview_viewport = viewport_create(context, 1, 1);

  s_sceneview_gbuffer_color_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * context->swapchain->image_count, 0, 0);
  s_sceneview_gbuffer_depth_attachment = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * context->swapchain->image_count, 0, 0);

  sceneview_create_attachments(context);
}
void sceneview_refresh(context_t *context) {
  if (s_sceneview_is_dirty) {

    s_sceneview_is_dirty = 0;

    sceneview_destroy_attachments(context);

    viewport_resize(s_sceneview_viewport, s_sceneview_width, s_sceneview_height);

    sceneview_create_attachments(context);
  }
}
void sceneview_draw(context_t *context) {
  if (dockspace_begin_child("SceneView", &s_sceneview_is_open, &s_sceneview_is_docked)) {

    ImGui::SetNextItemWidth(200.0F);

    if (ImGui::BeginCombo("Attachment", s_sceneview_gbuffer_attachment_names[s_sceneview_gbuffer_attachment_type])) {

      uint64_t attachment_index = 0;
      uint64_t attachment_count = ARRAY_COUNT(s_sceneview_gbuffer_attachment_names);

      while (attachment_index < attachment_count) {

        uint8_t is_selected = (s_sceneview_gbuffer_attachment_type == attachment_index);

        if (ImGui::Selectable(s_sceneview_gbuffer_attachment_names[attachment_index], is_selected)) {
          s_sceneview_gbuffer_attachment_type = (gbuffer_attachment_type_t)attachment_index;
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }

        attachment_index++;
      }

      ImGui::EndCombo();
    }

    ImVec2 window_size = ImGui::GetWindowSize();

    window_size.y -= 25.0F;

    s_sceneview_width = window_size.x < 0 ? 1 : (uint32_t)window_size.x;
    s_sceneview_height = window_size.y < 0 ? 1 : (uint32_t)window_size.y;
    s_sceneview_is_dirty = ((s_sceneview_width != s_sceneview_prev_width) || (s_sceneview_height != s_sceneview_prev_height));

    if (s_sceneview_is_dirty) {
      s_sceneview_prev_width = s_sceneview_width;
      s_sceneview_prev_height = s_sceneview_height;
      s_sceneview_is_dirty = 1;
    }

    ImVec2 image_position_min = ImGui::GetCursorScreenPos();
    ImVec2 image_position_max = ImVec2(image_position_min.x + window_size.x, image_position_min.y + window_size.y);

    VkDescriptorSet gbuffer_image = 0;

    switch (s_sceneview_gbuffer_attachment_type) {
      case GBUFFER_ATTACHMENT_TYPE_COLOR: {

        gbuffer_image = s_sceneview_gbuffer_color_attachment[context->renderer->image_index];

        break;
      }

      case GBUFFER_ATTACHMENT_TYPE_DEPTH: {

        gbuffer_image = s_sceneview_gbuffer_depth_attachment[context->renderer->image_index];

        break;
      }
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddImageRounded(gbuffer_image, image_position_min, image_position_max, ImVec2(0.0F, 0.0F), ImVec2(1.0F, 1.0F), IM_COL32_WHITE, 1.0F);

    dockspace_end_child(s_sceneview_is_docked);
  }
}
void sceneview_destroy(context_t *context) {
  sceneview_destroy_attachments(context);

  viewport_destroy(s_sceneview_viewport);

  heap_free(s_sceneview_gbuffer_color_attachment);
  heap_free(s_sceneview_gbuffer_depth_attachment);
}

static void sceneview_create_attachments(context_t *context) {
  uint64_t image_index = 0;
  uint64_t image_count = context->swapchain->image_count;

  while (image_index < image_count) {

    VkSampler gbuffer_color_sampler = s_sceneview_viewport->color_sampler[image_index];
    VkImageView gbuffer_color_image_view = s_sceneview_viewport->color_image_view[image_index];
    VkImageLayout gbuffer_color_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    s_sceneview_gbuffer_color_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_color_sampler, gbuffer_color_image_view, gbuffer_color_image_layout);

    VkSampler gbuffer_depth_sampler = s_sceneview_viewport->depth_sampler[image_index];
    VkImageView gbuffer_depth_image_view = s_sceneview_viewport->depth_image_view[image_index];
    VkImageLayout gbuffer_depth_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    s_sceneview_gbuffer_depth_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_depth_sampler, gbuffer_depth_image_view, gbuffer_depth_image_layout);

    image_index++;
  }
}
static void sceneview_destroy_attachments(context_t *context) {
  uint64_t image_index = 0;
  uint64_t image_count = context->swapchain->image_count;

  while (image_index < image_count) {

    ImGui_ImplVulkan_RemoveTexture(s_sceneview_gbuffer_color_attachment[image_index]);
    ImGui_ImplVulkan_RemoveTexture(s_sceneview_gbuffer_depth_attachment[image_index]);

    image_index++;
  }
}
