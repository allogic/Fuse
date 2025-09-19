#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

#include <engine/buffer.h>

typedef enum pipeline_type_t {
  PIPELINE_TYPE_NONE = 0,
  PIPELINE_TYPE_GRAPHIC,
  PIPELINE_TYPE_COMPUTE,
} pipeline_type_t;

typedef struct pipeline_t {
  pipeline_type_t type;
  string_t name;
  uint8_t *raw_vertex_buffer;
  uint8_t *raw_fragment_buffer;
  uint8_t *raw_compute_buffer;
  uint64_t raw_vertex_buffer_size;
  uint64_t raw_fragment_buffer_size;
  uint64_t raw_compute_buffer_size;
  SpvReflectShaderModule reflect_vertex_shader_module;
  SpvReflectShaderModule reflect_fragment_shader_module;
  SpvReflectShaderModule reflect_compute_shader_module;
  VkShaderModule vertex_shader_module;
  VkShaderModule fragment_shader_module;
  VkShaderModule compute_shader_module;
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  vector_t descriptor_sets;
  map_t descriptor_binding_name_to_index;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

pipeline_t pipeline_create(pipeline_type_t pipeline_type, char const *pipeline_name, ...);
void pipeline_link_buffer(pipeline_t *pipeline, int32_t frame_index, char const *binding_name, buffer_t *buffer);
void pipeline_allocate_descriptor_sets(pipeline_t *pipeline, uint64_t descriptor_count);
void pipeline_update_descriptor_sets(pipeline_t *pipeline);
void pipeline_destroy(pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PIPELINE_H
