#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

#include <stdint.h>

#include <library/core/api.h>

#include <spirv_reflect/spirv_reflect.h>

typedef struct pipeline_t {
  char pipeline_name[0xFF];
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
} pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

pipeline_t pipeline_alloc(char const *pipeline_name);
void pipeline_print(pipeline_t *pipeline);
void pipeline_free(pipeline_t *pipeline);

void pipeline_push_input_binding_descriptions(pipeline_t *pipeline, SpvReflectShaderModule *shader_module);
void pipeline_push_input_attribute_descriptions(pipeline_t *pipeline, SpvReflectShaderModule *shader_module);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_PIPELINE_H
