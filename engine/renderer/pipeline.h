#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

typedef enum pipline_type_t {
  PIPELINE_TYPE_NONE = 0,
  PIPELINE_TYPE_GRAPHIC,
  PIPELINE_TYPE_COMPUTE,
} pipline_type_t;

typedef struct pipeline_t {
  string_t name;
  pipline_type_t type;
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
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

pipeline_t pipeline_create_graphic(uint32_t frames_in_flight, char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path);
pipeline_t pipeline_create_compute(uint32_t frames_in_flight, char const *pipeline_name, char const *compute_shader_file_path);
void pipeline_allocate_descriptor_sets(pipeline_t *pipeline, uint32_t frames_in_flight);
void pipeline_update_descriptor_sets(pipeline_t *pipeline, uint32_t frames_in_flight);
void pipeline_free(pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_PIPELINE_H
