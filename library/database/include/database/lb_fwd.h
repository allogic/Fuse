#ifndef LB_DATABASE_FWD_H
#define LB_DATABASE_FWD_H

typedef int64_t lb_swapchain_asset_id_t;

typedef int64_t lb_renderer_asset_id_t;

typedef int64_t lb_pipeline_asset_id_t;
typedef int64_t lb_pipeline_resource_id_t;
typedef int64_t lb_pipeline_vertex_input_binding_id_t;
typedef int64_t lb_pipeline_descriptor_binding_id_t;

typedef int64_t lb_model_asset_id_t;
typedef int64_t lb_model_resource_id_t;
typedef int64_t lb_model_mesh_id_t;

typedef int64_t lb_mesh_primitive_id_t;
typedef int64_t lb_mesh_attribute_id_t;

typedef int64_t lb_attribute_data_id_t;

typedef int64_t lb_scene_asset_id_t;

typedef int64_t lb_graph_asset_id_t;

typedef int64_t lb_node_asset_id_t;

typedef int64_t lb_mesh_node_id_t;
typedef int64_t lb_noise_node_id_t;
typedef int64_t lb_output_node_id_t;

typedef enum lb_pipeline_type_t {
  LB_PIPELINE_TYPE_GRAPHIC,
  LB_PIPELINE_TYPE_COMPUTE,
  LB_PIPELINE_TYPE_COUNT,
} lb_pipeline_type_t;

typedef enum lb_node_type_t {
  LB_NODE_TYPE_MESH,
  LB_NODE_TYPE_NOISE,
  LB_NODE_TYPE_OUTPUT,
  LB_NODE_TYPE_COUNT,
} lb_node_type_t;

typedef struct lb_swapchain_asset_t {
  lb_swapchain_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
  uint32_t image_count;
  uint32_t depth_format;
  uint8_t is_default;
} lb_swapchain_asset_t;

typedef struct lb_renderer_asset_t {
  lb_renderer_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
  uint32_t frames_in_flight;
  uint8_t is_default;
} lb_renderer_asset_t;

typedef struct lb_pipeline_asset_t {
  lb_pipeline_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
  lb_pipeline_type_t type;
  uint32_t link_index;
  uint8_t auto_create_pipeline;
  uint8_t auto_create_vertex_input_buffer; // TODO: experimental..
  uint8_t auto_link_descriptor_bindings;
  uint8_t interleaved_vertex_input_buffer;
} lb_pipeline_asset_t;
typedef struct lb_pipeline_resource_t {
  lb_pipeline_resource_id_t id;
  lb_pipeline_asset_id_t pipeline_asset_id;
  char vertex_shader_file_path[LB_FILE_PATH_SIZE];
  char fragment_shader_file_path[LB_FILE_PATH_SIZE];
  char compute_shader_file_path[LB_FILE_PATH_SIZE];
  uint64_t vertex_shader_file_path_size;
  uint64_t fragment_shader_file_path_size;
  uint64_t compute_shader_file_path_size;
} lb_pipeline_resource_t;
typedef struct lb_pipeline_vertex_input_binding_t {
  lb_pipeline_vertex_input_binding_id_t id;
  lb_pipeline_asset_id_t pipeline_asset_id;
  char name[LB_ASSET_NAME_SIZE];
  uint32_t location;
  uint32_t size;
  uint32_t component_count;
  uint32_t format;
  uint32_t input_rate;
} lb_pipeline_vertex_input_binding_t;
typedef struct lb_pipeline_descriptor_binding_t {
  lb_pipeline_descriptor_binding_id_t id;
  lb_pipeline_asset_id_t pipeline_asset_id;
  char name[LB_ASSET_NAME_SIZE];
  uint32_t binding;
  uint32_t descriptor_type;
  uint32_t descriptor_count;
  uint32_t stage_flags;
  uint8_t auto_buffer;
} lb_pipeline_descriptor_binding_t;

typedef struct lb_model_asset_t {
  lb_model_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
} lb_model_asset_t;
typedef struct lb_model_resource_t {
  lb_model_resource_id_t id;
  lb_model_asset_id_t model_asset_id;
  char model_file_path[LB_FILE_PATH_SIZE];
} lb_model_resource_t;
typedef struct lb_model_mesh_t {
  lb_model_mesh_id_t id;
  lb_model_asset_id_t model_asset_id;
  char name[LB_ASSET_NAME_SIZE];
} lb_model_mesh_t;

typedef struct lb_mesh_primitive_t {
  lb_mesh_primitive_id_t id;
  lb_model_mesh_id_t model_mesh_id;
  char name[LB_ASSET_NAME_SIZE];
} lb_mesh_primitive_t;
typedef struct lb_mesh_attribute_t {
  lb_mesh_attribute_id_t id;
  lb_mesh_primitive_id_t mesh_primitive_id;
  char name[LB_ASSET_NAME_SIZE];
  uint32_t type;
  uint64_t count;
} lb_mesh_attribute_t;

typedef struct lb_attribute_buffer_t {
  lb_attribute_data_id_t id;
  lb_mesh_attribute_id_t mesh_attribute_id;
  uint8_t *data;
  uint64_t data_size;
} lb_attribute_buffer_t;

typedef struct lb_scene_asset_t {
  lb_scene_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
} lb_scene_asset_t;

typedef struct lb_graph_asset_t {
  lb_graph_asset_id_t id;
  char name[LB_ASSET_NAME_SIZE];
  uint64_t name_size;
  uint32_t unique_node_id;
  uint32_t unique_pin_id;
  uint32_t unique_link_id;
} lb_graph_asset_t;

typedef struct lb_noise_node_t {
  lb_noise_node_id_t id;
  uint32_t factor_pin_id;
  float position_x;
  float position_y;
} lb_noise_node_t;
typedef struct lb_mesh_node_t {
  lb_mesh_node_id_t id;
  uint32_t mesh_pin_id;
  float position_x;
  float position_y;
} lb_mesh_node_t;
typedef struct lb_output_node_t {
  lb_output_node_id_t id;
  uint32_t mesh_pin_id;
  float position_x;
  float position_y;
} lb_output_node_t;

#endif // LB_DATABASE_FWD_H
