#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

#include <library/core/api.h>

typedef enum pipline_type_t {
  PIPELINE_TYPE_NONE = 0,
  PIPELINE_TYPE_GRAPHIC,
  PIPELINE_TYPE_COMPUTE,
} pipline_type_t;

typedef struct pipeline_t {
  char pipeline_name[0xFF];
  pipline_type_t pipeline_type;
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

pipeline_t pipeline_create_graphic(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path);
pipeline_t pipeline_create_compute(char const *pipeline_name, char const *compute_shader_file_path);
void pipeline_print(pipeline_t *pipeline);
void pipeline_free(pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_PIPELINE_H
