#ifndef EG_FWD_H
#define EG_FWD_H

typedef struct buffer_t {
  uint64_t buffer_size;
  VkBuffer buffer;
  VkDeviceMemory device_memory;
} buffer_t;

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
  pipeline_resource_t resource;
  uint64_t vertex_input_binding_count;
  uint64_t descriptor_binding_count;
  uint64_t descriptor_pool_size_count;
  uint64_t descriptor_set_count;
  uint8_t interleaved_vertex_input_buffer;
  VkBuffer **vertex_input_binding_buffers_per_frame;
  uint64_t **vertex_input_binding_offsets_per_frame;
  VkBuffer **descriptor_binding_buffers_per_frame;
  VkBuffer *index_buffer_per_frame;
  map_t *auto_link_descriptor_binding_buffers_per_frame;
  VkVertexInputBindingDescription *vertex_input_binding_descriptions;
  VkVertexInputAttributeDescription *vertex_input_attribute_descriptions;
  VkDescriptorPoolSize *descriptor_pool_sizes;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  vector_t vertex_input_bindings;
  vector_t descriptor_bindings;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} graphic_pipeline_t;
typedef struct compute_pipeline_t {
  pipeline_resource_t resource;
  uint64_t descriptor_binding_count;
  uint64_t descriptor_pool_size_count;
  VkBuffer **descriptor_binding_buffers_per_frame;
  map_t *auto_link_descriptor_binding_buffers_per_frame;
  VkDescriptorPoolSize *descriptor_pool_sizes;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  vector_t descriptor_bindings;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} compute_pipeline_t;

typedef struct debug_vertex_t {
  vector3_t world_position;
  vector4_t color;
} debug_vertex_t;

#endif // EG_FWD_H
