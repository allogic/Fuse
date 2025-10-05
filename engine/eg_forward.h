#ifndef EG_FORWARD_H
#define EG_FORWARD_H

typedef struct buffer_t {
  uint64_t buffer_size;
  VkBuffer buffer;
  VkDeviceMemory device_memory;
} buffer_t;

typedef struct camera_t {
  float fov;
  float near_z;
  float far_z;
} camera_t;

typedef struct transform_t {
  struct transform_t *parent;
  struct transform_t **children;
  uint64_t child_count;
  vector3_t local_right;
  vector3_t local_up;
  vector3_t local_front;
  vector3_t local_left;
  vector3_t local_down;
  vector3_t local_back;
  vector3_t local_position;
  quaternion_t local_rotation;
  vector3_t local_scale;
  vector3_t world_position;
  quaternion_t world_rotation;
  vector3_t world_scale;
} transform_t;

typedef struct player_t {
  camera_t *camera;
  transform_t *transform;
  vector3_t velocity;
  vector3_t mouse_begin;
  vector3_t mouse_end;
  vector3_t mouse_delta;
  float keyboard_move_speed_fast;
  float keyboard_move_speed_normal;
  float mouse_move_speed_fast;
  float mouse_move_speed_normal;
  float mouse_rotation_speed;
  float move_drag;
  float rotation_drag;
} player_t;

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;
typedef struct screen_info_t {
  vector2_t resolution;
} screen_info_t;
typedef struct camera_info_t {
  vector3_t world_position;
  int32_t reserved;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
} camera_info_t;

typedef struct graphic_pipeline_t {
  uint64_t vertex_input_binding_count;
  uint64_t descriptor_binding_count;
  // uint64_t descriptor_count;
  vector_t vertex_input_binding_buffers;
  vector_t vertex_input_binding_offsets;
  vector_t index_buffers;
  vector_t descriptor_binding_buffers;
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  vector_t descriptor_sets;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} graphic_pipeline_t;

typedef struct compute_pipeline_t {
  uint32_t frames_in_flight;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} compute_pipeline_t;

#endif // EG_FORWARD_H
