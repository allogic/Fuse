#include <editor/ed_pch.h>

#include <editor/view/ed_viewport.h>

extern char const *g_viewport_gbuffer_attachment_type_names[ED_GBUFFER_ATTACHMENT_TYPE_COUNT] = {
  "Color",
  "Depth",
};

ed_viewport_t::ed_viewport_t(char const *name, uint8_t enable_controls) : ed_view_t(name),
                                                                          m_enable_controls(enable_controls) {
  uint32_t image_count = eg_swapchain_image_count();

  m_gbuffer_color_attachment = (VkDescriptorSet *)eg_heap_alloc(sizeof(VkDescriptorSet) * image_count, 0, 0);
  m_gbuffer_depth_attachment = (VkDescriptorSet *)eg_heap_alloc(sizeof(VkDescriptorSet) * image_count, 0, 0);

  create_attachments();
}
ed_viewport_t::~ed_viewport_t() {
  destroy_attachments();

  eg_viewport_destroy(m_viewport);

  eg_heap_free(m_gbuffer_color_attachment);
  eg_heap_free(m_gbuffer_depth_attachment);
}

void ed_viewport_t::refresh() {
  if (m_is_dirty) {

    m_is_dirty = 0;

    destroy_attachments();

    eg_viewport_resize(m_viewport, m_width, m_height);

    create_attachments();
  }
}
void ed_viewport_t::draw() {
  uint32_t image_index = eg_renderer_image_index();

  if (m_enable_controls) {

    ImGui::SetNextItemWidth(200.0F);

    if (ImGui::BeginCombo("Attachment", g_viewport_gbuffer_attachment_type_names[m_gbuffer_attachment_type])) {

      uint64_t attachment_index = 0;
      uint64_t attachment_count = ED_GBUFFER_ATTACHMENT_TYPE_COUNT;

      while (attachment_index < attachment_count) {

        uint8_t is_selected = (m_gbuffer_attachment_type == attachment_index);

        if (ImGui::Selectable(g_viewport_gbuffer_attachment_type_names[attachment_index], is_selected)) {
          m_gbuffer_attachment_type = (ed_gbuffer_attachment_type_t)attachment_index;
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

  if (m_enable_controls) {
    window_size.y -= 25.0F;
  }

  m_width = window_size.x < 0 ? 1 : (uint32_t)window_size.x;
  m_height = window_size.y < 0 ? 1 : (uint32_t)window_size.y;

  m_is_dirty = ((m_width != m_prev_width) || (m_height != m_prev_height));

  if (m_is_dirty) {

    m_is_dirty = 1;

    m_prev_width = m_width;
    m_prev_height = m_height;
  }

  ImVec2 image_position_min = ImGui::GetCursorScreenPos();
  ImVec2 image_position_max = ImVec2(image_position_min.x + window_size.x, image_position_min.y + window_size.y);

  VkDescriptorSet gbuffer_image = 0;

  switch (m_gbuffer_attachment_type) {
    case ED_GBUFFER_ATTACHMENT_TYPE_COLOR: {

      gbuffer_image = m_gbuffer_color_attachment[image_index];

      break;
    }

    case ED_GBUFFER_ATTACHMENT_TYPE_DEPTH: {

      gbuffer_image = m_gbuffer_depth_attachment[image_index];

      break;
    }
  }

  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  draw_list->AddImageRounded(gbuffer_image, image_position_min, image_position_max, ImVec2(0.0F, 0.0F), ImVec2(1.0F, 1.0F), IM_COL32_WHITE, 1.0F);
}

void ed_viewport_t::create_attachments() {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    VkSampler gbuffer_color_sampler = eg_viewport_color_sampler(m_viewport, image_index);
    VkImageView gbuffer_color_image_view = eg_viewport_color_image_view(m_viewport, image_index);
    VkImageLayout gbuffer_color_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    m_gbuffer_color_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_color_sampler, gbuffer_color_image_view, gbuffer_color_image_layout);

    VkSampler gbuffer_depth_sampler = eg_viewport_depth_sampler(m_viewport, image_index);
    VkImageView gbuffer_depth_image_view = eg_viewport_depth_image_view(m_viewport, image_index);
    VkImageLayout gbuffer_depth_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    m_gbuffer_depth_attachment[image_index] = ImGui_ImplVulkan_AddTexture(gbuffer_depth_sampler, gbuffer_depth_image_view, gbuffer_depth_image_layout);

    image_index++;
  }
}
void ed_viewport_t::destroy_attachments() {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    ImGui_ImplVulkan_RemoveTexture(m_gbuffer_color_attachment[image_index]);
    ImGui_ImplVulkan_RemoveTexture(m_gbuffer_depth_attachment[image_index]);

    image_index++;
  }
}
