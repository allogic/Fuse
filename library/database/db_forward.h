#ifndef DB_FORWARD_H
#define DB_FORWARD_H

typedef int64_t swapchain_asset_id_t;

typedef int64_t renderer_asset_id_t;

typedef int64_t pipeline_asset_id_t;
typedef int64_t pipeline_resource_id_t;
typedef int64_t pipeline_vertex_input_binding_id_t;
typedef int64_t pipeline_descriptor_binding_id_t;

typedef int64_t model_asset_id_t;
typedef int64_t model_resource_id_t;
typedef int64_t model_mesh_id_t;
typedef int64_t model_primitive_id_t;
typedef int64_t model_attribute_id_t;

typedef struct swapchain_asset_t {
  swapchain_asset_id_t id;
  char *name;
  uint32_t image_count;
  uint32_t depth_format;
} swapchain_asset_t;

typedef struct renderer_asset_t {
  renderer_asset_id_t id;
  char *name;
  uint32_t frames_in_flight;
} renderer_asset_t;

typedef struct pipeline_asset_t {
  pipeline_asset_id_t id;
  char *name;
  uint32_t type;
  uint32_t link_index;
  uint8_t auto_create;
  uint8_t auto_vertex_input_buffer;
  uint8_t interleaved_vertex_input_buffer;
} pipeline_asset_t;
typedef struct pipeline_resource_t {
  pipeline_resource_id_t id;
  pipeline_asset_id_t pipeline_asset_id;
  char *vertex_shader_file_path;
  char *fragment_shader_file_path;
  char *compute_shader_file_path;
} pipeline_resource_t;
typedef struct pipeline_vertex_input_binding_t {
  pipeline_vertex_input_binding_id_t id;
  pipeline_asset_id_t pipeline_asset_id;
  char *binding_name;
  uint32_t location;
  uint32_t size;
  uint32_t component_count;
  uint32_t format;
  uint32_t input_rate;
} pipeline_vertex_input_binding_t;
typedef struct pipeline_descriptor_binding_t {
  pipeline_descriptor_binding_id_t id;
  pipeline_asset_id_t pipeline_asset_id;
  char *binding_name;
  uint32_t binding;
  uint32_t descriptor_type;
  uint32_t descriptor_count;
  uint32_t stage_flags;
  uint8_t auto_buffer;
} pipeline_descriptor_binding_t;

typedef struct model_asset_t {
  model_asset_id_t id;
  char *name;
} model_asset_t;
typedef struct model_resource_t {
  model_resource_id_t id;
  model_asset_id_t model_asset_id;
  char *model_file_path;
} model_resource_t;
typedef struct model_mesh_t {
  model_mesh_id_t id;
  model_asset_id_t model_asset_id;
} model_mesh_t;
typedef struct model_primitive_t {
  model_primitive_id_t id;
  model_mesh_id_t model_mesh_id;
} model_primitive_t;
typedef struct model_attribute_t {
  model_attribute_id_t id;
  model_primitive_id_t model_primitive_id;
  char *name;
  uint32_t type;
} model_attribute_t;

#endif // DB_FORWARD_H
