#ifndef ED_FWD_H
#define ED_FWD_H

typedef enum asset_type_t {
  ASSET_TYPE_NONE = 0,
  ASSET_TYPE_SWAPCHAIN,
  ASSET_TYPE_RENDERER,
  ASSET_TYPE_PIPELINE,
  ASSET_TYPE_MODEL,
} asset_type_t;

typedef enum gbuffer_attachment_type_t {
  GBUFFER_ATTACHMENT_TYPE_COLOR,
  GBUFFER_ATTACHMENT_TYPE_DEPTH,
} gbuffer_attachment_type_t;

typedef struct sceneview_t {
  context_t *context;
  uint64_t link_index;
  uint32_t width;
  uint32_t height;
  uint32_t prev_width;
  uint32_t prev_height;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
  char name[0xFF];
  viewport_t *viewport;
  VkDescriptorSet *gbuffer_color_attachment;
  VkDescriptorSet *gbuffer_depth_attachment;
  gbuffer_attachment_type_t gbuffer_attachment_type;
} sceneview_t;

#endif // ED_FWD_H
