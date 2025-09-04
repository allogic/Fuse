#include <stdio.h>
#include <string.h>

#include <spirv_reflect/spirv_reflect.h>

#include <library/core/api.h>

#include <enton/enton.h>
#include <enton/context.h>
#include <enton/macros.h>

static void enton_print_input_variables(SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint32_t input_variable_index = 0;
  while (input_variable_index < input_variable_count) {
    printf("Input %s at location %u\n",
           input_variables[input_variable_index]->name,
           input_variables[input_variable_index]->location);

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void enton_print_descriptor_bindings(SpvReflectShaderModule *shader_module) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    printf("Set %u, Binding %u: %s\n",
           descriptor_bindings[descriptor_binding_index]->set,
           descriptor_bindings[descriptor_binding_index]->binding,
           descriptor_bindings[descriptor_binding_index]->name);

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}

int32_t main(int32_t argc, char **argv) {
  if (strcmp("render", "render") == 0) {
    uint8_t *vertex_buffer = 0;
    uint8_t *fragment_buffer = 0;

    uint64_t vertex_buffer_size = 0;
    uint64_t fragment_buffer_size = 0;

    filesys_load_binary(&vertex_buffer, &vertex_buffer_size, "C:\\Users\\burm\\Downloads\\Fuse\\shader\\chunk\\renderer.vert.spv");
    filesys_load_binary(&fragment_buffer, &fragment_buffer_size, "C:\\Users\\burm\\Downloads\\Fuse\\shader\\chunk\\renderer.frag.spv");

    SpvReflectShaderModule vertex_shader_module = {0};
    SpvReflectShaderModule fragment_shader_module = {0};

    SPIRV_CHECK(spvReflectCreateShaderModule(vertex_buffer_size, vertex_buffer, &vertex_shader_module));
    SPIRV_CHECK(spvReflectCreateShaderModule(fragment_buffer_size, fragment_buffer, &fragment_shader_module));

    context_build_render_pipeline(&vertex_shader_module, &fragment_shader_module, "chunk_renderer");

    spvReflectDestroyShaderModule(&vertex_shader_module);
    spvReflectDestroyShaderModule(&fragment_shader_module);

    heap_free(vertex_buffer);
    heap_free(fragment_buffer);
  } else if (strcmp("compute", argv[1]) == 0) {
    uint8_t *compute_buffer = 0;

    uint64_t compute_buffer_size = 0;

    filesys_load_binary(&compute_buffer, &compute_buffer_size, argv[3]);

    SpvReflectShaderModule compute_shader_module = {0};

    SPIRV_CHECK(spvReflectCreateShaderModule(compute_buffer_size, compute_buffer, &compute_shader_module));

    context_build_compute_pipeline(&compute_shader_module, argv[2]);

    spvReflectDestroyShaderModule(&compute_shader_module);

    heap_free(compute_buffer);
  }

  heap_reset();

  return 0;
}
