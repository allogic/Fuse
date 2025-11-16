#ifndef ED_FWD_H
#define ED_FWD_H

typedef enum ed_dockspace_type_t {
  ED_DOCKSPACE_TYPE_GAME,
  ED_DOCKSPACE_TYPE_SCENE,
  ED_DOCKSPACE_TYPE_MODEL,
  ED_DOCKSPACE_TYPE_COUNT,
} ed_dockspace_type_t;

typedef enum ed_gbuffer_attachment_type_t {
  ED_GBUFFER_ATTACHMENT_TYPE_COLOR,
  ED_GBUFFER_ATTACHMENT_TYPE_DEPTH,
  ED_GBUFFER_ATTACHMENT_TYPE_COUNT,
} ed_gbuffer_attachment_type_t;

typedef struct ed_viewport_t {
  eg_context_t *context;
  uint32_t width;
  uint32_t height;
  uint32_t prev_width;
  uint32_t prev_height;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
  eg_viewport_t *viewport;
  VkDescriptorSet *gbuffer_color_attachment;
  VkDescriptorSet *gbuffer_depth_attachment;
  ed_gbuffer_attachment_type_t gbuffer_attachment_type;
} ed_viewport_t;

typedef struct ed_catalog_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
  int64_t selected_asset = -1;
  asset_type_t asset_type;
  vector_t assets;
} ed_catalog_t;

typedef struct ed_hierarchy_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
  ecs_entity_t selected_entity;
} ed_hierarchy_t;

typedef struct ed_inspector_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
} ed_inspector_t;

typedef struct ed_profiler_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
} ed_profiler_t;

#endif // ED_FWD_H
