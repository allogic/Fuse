#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

#include <enton/forward.h>

#include <spirv_reflect/spirv_reflect.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void context_build_render_pipeline(SpvReflectShaderModule *vertex_shader_module, SpvReflectShaderModule *fragment_shader_module, const char *pipeline_name);
void context_build_compute_pipeline(SpvReflectShaderModule *compute_shader_module, const char *pipeline_name);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONTEXT_H
