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

typedef enum ed_view_type_t {
  ED_VIEW_TYPE_NONE,
  ED_VIEW_TYPE_CANVAS,
  ED_VIEW_TYPE_VIEWPORT,
  ED_VIEW_TYPE_CATALOG,
  ED_VIEW_TYPE_HIERARCHY,
  ED_VIEW_TYPE_INSPECTOR,
  ED_VIEW_TYPE_PROFILER,
} ed_view_type_t;

typedef enum ed_node_type_t {
  ED_NODE_TYPE_NONE,
  ED_NODE_TYPE_NOISE,
  ED_NODE_TYPE_COUNT,
} ed_node_type_t;

typedef struct ed_view_t {
  eg_context_t *context;
  ed_view_type_t type;
  uint8_t is_dirty;
  uint8_t is_open;
  uint8_t is_docked;
} ed_view_t;
typedef struct ed_node_t {
  eg_context_t *context;
  ed_node_type_t type;
  uint32_t id;
} ed_node_t;

typedef struct ed_canvas_view_t {
  ed_view_t base;
  uint32_t unique_node_id;
  uint32_t unique_pin_id;
  uint32_t unique_link_id;
  vector_t nodes;
  uint8_t reset_view_is_prending;
  uint8_t create_node_is_pending;
  ed_node_type_t next_node_type;
  float next_node_screen_position_y;
  float next_node_screen_position_x;
} ed_canvas_view_t;
typedef struct ed_viewport_view_t {
  ed_view_t base;
  uint32_t width;
  uint32_t height;
  uint32_t prev_width;
  uint32_t prev_height;
  eg_viewport_t *handle;
  VkDescriptorSet *gbuffer_color_attachment;
  VkDescriptorSet *gbuffer_depth_attachment;
  ed_gbuffer_attachment_type_t gbuffer_attachment_type;
} ed_viewport_view_t;
typedef struct ed_catalog_view_t {
  ed_view_t base;
  int64_t selected_asset;
  asset_type_t asset_type;
  vector_t assets;
} ed_catalog_view_t;
typedef struct ed_hierarchy_view_t {
  ed_view_t base;
  ecs_entity_t selected_entity;
} ed_hierarchy_view_t;
typedef struct ed_inspector_view_t {
  ed_view_t base;
} ed_inspector_view_t;
typedef struct ed_profiler_view_t {
  ed_view_t base;
} ed_profiler_view_t;

typedef struct ed_noise_node_t {
  ed_node_t base;
} ed_noise_node_t;

#endif // ED_FWD_H
