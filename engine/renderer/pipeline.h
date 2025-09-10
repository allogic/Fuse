#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

#include <library/core/api.h>

typedef struct graphic_pipeline_t {
  char pipeline_name[0xFF];
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_binding;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} graphic_pipeline_t;

typedef struct compute_pipeline_t {
  char pipeline_name[0xFF];
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_binding;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} compute_pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

graphic_pipeline_t graphic_pipeline_alloc(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path);
void graphic_pipeline_print(graphic_pipeline_t *pipeline);
void graphic_pipeline_free(graphic_pipeline_t *pipeline);

compute_pipeline_t compute_pipeline_alloc(char const *pipeline_name, char const *compute_shader_file_path);
void compute_pipeline_print(compute_pipeline_t *pipeline);
void compute_pipeline_free(compute_pipeline_t *pipeline);

void pipeline_push_input_binding_descriptions(vector_t *vertex_input_binding_descriptions, SpvReflectShaderModule *shader_module);
void pipeline_push_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions, SpvReflectShaderModule *shader_module);
void pipeline_push_descriptor_bindings(vector_t *descriptor_pool_sizes, SpvReflectShaderModule *shader_module);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_PIPELINE_H
