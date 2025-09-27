#ifndef DB_FORWARD_H
#define DB_FORWARD_H

typedef struct swapchain_asset_t {
  int32_t id;
  string_t name;
  uint32_t image_count;
  uint32_t depth_format;
} swapchain_asset_t;

typedef struct renderer_asset_t {
  int32_t id;
  string_t name;
  uint32_t frames_in_flight;
} renderer_asset_t;

typedef struct pipeline_asset_t {
  int32_t id;
  string_t name;
  uint32_t type;
  uint32_t link_index;
  uint8_t auto_create;
} pipeline_asset_t;

typedef struct pipeline_resource_t {
  int32_t id;
  int32_t pipeline_id;
  vector_t vertex_shader;
  vector_t fragment_shader;
  vector_t compute_shader;
} pipeline_resource_t;

typedef struct pipeline_vertex_input_binding_t {
  int32_t id;
  int32_t pipeline_id;
  string_t binding_name;
  uint32_t binding;
  uint32_t location;
  uint32_t stride;
  uint32_t format;
  uint32_t offset;
  uint32_t input_rate;
} pipeline_vertex_input_binding_t;

typedef struct pipeline_descriptor_set_layout_binding_t {
  int32_t id;
  int32_t pipeline_id;
  string_t binding_name;
  uint32_t binding;
  uint32_t descriptor_type;
  uint32_t descriptor_count;
  uint32_t stage_flags;
  uint8_t auto_buffer;
} pipeline_descriptor_set_layout_binding_t;

#endif // DB_FORWARD_H
