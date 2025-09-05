#include "stdio.h"
#include "string.h"

#include <spirv_reflect/spirv_reflect.h>

#include <library/core/api.h>

#include <enton/context.h>
#include <enton/macros.h>

static void context_append_input_variables(string_t *pipeline, SpvReflectShaderModule *shader_module, const char *pipeline_name);

void context_build_render_pipeline(SpvReflectShaderModule *vertex_shader_module, SpvReflectShaderModule *fragment_shader_module, const char *pipeline_name) {
  string_t pipeline_name_upper = string_from(pipeline_name);

  string_upper(&pipeline_name_upper);

  string_t pipeline_header_name = string_from(pipeline_name);
  string_t pipeline_source_name = string_from(pipeline_name);

  string_appendf(&pipeline_header_name, ".h");
  string_appendf(&pipeline_source_name, ".c");

  string_t pipeline_header = string_alloc();
  string_t pipeline_source = string_alloc();

  string_appendf(&pipeline_header, "#ifndef %s_H\n", string_buffer(&pipeline_name_upper));
  string_appendf(&pipeline_header, "#define %s\n", string_buffer(&pipeline_name_upper));
  string_appendf(&pipeline_header, "\n");

  // context_append_input_variables(&pipeline_header, vertex_shader_module);

  string_appendf(&pipeline_header, "\n");
  string_appendf(&pipeline_header, "#endif // %s_H\n", string_buffer(&pipeline_name_upper));

  string_appendf(&pipeline_source, "#include <engine/renderer/%s.h>\n", string_buffer(&pipeline_name_upper));
  string_appendf(&pipeline_source, "\n");

  context_append_input_variables(&pipeline_header, vertex_shader_module, pipeline_name);

  string_appendf(&pipeline_source, "\n");

  string_to_file(&pipeline_header, string_buffer(&pipeline_header_name));
  string_to_file(&pipeline_source, string_buffer(&pipeline_source_name));

  string_free(&pipeline_name_upper);

  string_free(&pipeline_header_name);
  string_free(&pipeline_source_name);

  string_free(&pipeline_header);
  string_free(&pipeline_source);
}
void context_build_compute_pipeline(SpvReflectShaderModule *compute_shader_module, const char *pipeline_name) {
}

static void context_append_input_variables(string_t *pipeline, SpvReflectShaderModule *shader_module, const char *pipeline_name) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  string_appendf(pipeline, "static VkVertexInputBindingDescription const s_%s_vertex_input_binding_descriptions[] = {\n", pipeline_name);

  uint32_t input_variable_index = 0;
  while (input_variable_index < input_variable_count) {
    string_appendf(pipeline, "{0, sizeof(uint32_t), VK_VERTEX_INPUT_RATE_VERTEX},");

    printf("Input %s at location %u\n",
           input_variables[input_variable_index]->name,
           input_variables[input_variable_index]->location);

    input_variable_index++;
  }

  string_appendf(pipeline, "};\n");
  string_appendf(pipeline, "\n");

  heap_free(input_variables);
}
