#ifndef ED_FWD_H
#define ED_FWD_H

typedef struct ed_graph_t {
  void *dummy;
} ed_graph_t;

/*
typedef struct ed_canvas_view_t {
  ed_view_t base;
  uint8_t reset_view_is_pending;
  uint8_t create_node_is_pending;
  float next_node_screen_position_y;
  float next_node_screen_position_x;
  ed_graph_t graph;
  eg_node_type_t next_node_type;
  eg_vector_t *selected_nodes;
} ed_canvas_view_t;
typedef struct ed_catalog_view_t {
  ed_view_t base;
  int64_t selected_asset;
  eg_asset_type_t asset_type;
  eg_vector_t *assets;
} ed_catalog_view_t;
typedef struct ed_geometry_view_t {
  ed_view_t base;
} ed_geometry_view_t;
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
*/

#endif // ED_FWD_H
