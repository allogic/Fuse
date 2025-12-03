#ifndef ED_VIEWPORT_VIEW_H
#define ED_VIEWPORT_VIEW_H

typedef enum ed_gbuffer_attachment_type_t {
  ED_GBUFFER_ATTACHMENT_TYPE_COLOR,
  ED_GBUFFER_ATTACHMENT_TYPE_DEPTH,
  ED_GBUFFER_ATTACHMENT_TYPE_COUNT,
} ed_gbuffer_attachment_type_t;

extern char const *g_viewport_gbuffer_attachment_type_names[ED_GBUFFER_ATTACHMENT_TYPE_COUNT];

class ed_viewport_t : public ed_view_t {
public:
  ed_viewport_t(char const *name, uint8_t enable_controls);
  virtual ~ed_viewport_t();

public:
  virtual void refresh() override;
  virtual void draw() override;

private:
  void create_attachments();
  void destroy_attachments();

private:
  eg_viewport_t *m_viewport = eg_viewport_create(1, 1);

  uint32_t m_width = 1;
  uint32_t m_height = 1;
  uint32_t m_prev_width = 1;
  uint32_t m_prev_height = 1;

  uint8_t m_enable_controls = 0;

  VkDescriptorSet *m_gbuffer_color_attachment = 0;
  VkDescriptorSet *m_gbuffer_depth_attachment = 0;

  ed_gbuffer_attachment_type_t m_gbuffer_attachment_type = ED_GBUFFER_ATTACHMENT_TYPE_COUNT;
};

#endif // ED_VIEWPORT_VIEW_H
