#include <library/lb_pch.h>

#include <library/core/co_api.h>

#include <library/database/db_importer.h>
#include <library/database/db_itself.h>
#include <library/database/db_macros.h>

static void db_importer_process_vertex_input_bindings(int32_t pipeline_id, SpvReflectShaderModule *shader_module);
static void db_importer_process_descriptor_set_layout_bindings(int32_t pipeline_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag);

void db_importer_insert_graphic_pipeline(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path) {
  SpvReflectShaderModule reflect_vertex_shader_module = {0};
  SpvReflectShaderModule reflect_fragment_shader_module = {0};

  uint8_t *vertex_shader = 0;
  uint8_t *fragment_shader = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesys_load_binary(&vertex_shader, &vertex_shader_size, vertex_shader_file_path);
  filesys_load_binary(&fragment_shader, &fragment_shader_size, fragment_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(vertex_shader_size, vertex_shader, &reflect_vertex_shader_module));
  SPIRV_CHECK(spvReflectCreateShaderModule(fragment_shader_size, fragment_shader, &reflect_fragment_shader_module));

  int32_t pipeline_id = db_store_pipeline_asset(pipeline_name, 0, 0, 1);

  db_store_pipeline_resource(pipeline_id, vertex_shader, vertex_shader_size, fragment_shader, fragment_shader_size, 0, 0);

  db_importer_process_vertex_input_bindings(pipeline_id, &reflect_vertex_shader_module);
  db_importer_process_descriptor_set_layout_bindings(pipeline_id, &reflect_vertex_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  db_importer_process_descriptor_set_layout_bindings(pipeline_id, &reflect_fragment_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

  spvReflectDestroyShaderModule(&reflect_vertex_shader_module);
  spvReflectDestroyShaderModule(&reflect_fragment_shader_module);

  heap_free(vertex_shader);
  heap_free(fragment_shader);
}
void db_importer_insert_compute_pipeline(char const *pipeline_name, char const *compute_shader_file_path) {
  SpvReflectShaderModule reflect_compute_shader_module = {0};

  uint8_t *compute_shader = 0;

  uint64_t compute_shader_size = 0;

  filesys_load_binary(&compute_shader, &compute_shader_size, compute_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(compute_shader_size, compute_shader, &reflect_compute_shader_module));

  int32_t pipeline_id = db_store_pipeline_asset(pipeline_name, 1, 0, 1);

  db_store_pipeline_resource(pipeline_id, 0, 0, 0, 0, compute_shader, compute_shader_size);

  db_importer_process_descriptor_set_layout_bindings(pipeline_id, &reflect_compute_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

  spvReflectDestroyShaderModule(&reflect_compute_shader_module);

  heap_free(compute_shader);
}

static void db_importer_process_vertex_input_bindings(int32_t pipeline_id, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint64_t input_variable_index = 0;
  uint32_t byte_offset = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    db_store_pipeline_vertex_input_binding(pipeline_id, input_variable->name, 0, input_variable->location, 0, input_variable->format, byte_offset, VK_VERTEX_INPUT_RATE_VERTEX);

    byte_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void db_importer_process_descriptor_set_layout_bindings(int32_t pipeline_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    db_store_pipeline_descriptor_set_layout_binding(pipeline_id, descriptor_binding->name, descriptor_binding->binding, descriptor_binding->descriptor_type, 1, stage_flag, 1);

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}
