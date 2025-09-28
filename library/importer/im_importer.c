#include <library/lb_pch.h>

#include <library/core/co_api.h>
#include <library/database/db_api.h>

#include <spirv_reflect/spirv_reflect.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

static void importer_process_vertex_input_bindings(pipeline_id_t pipeline_id, SpvReflectShaderModule *shader_module);
static void importer_process_descriptor_bindings(pipeline_id_t pipeline_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag);

void importer_import_graphic_pipeline(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path) {
  SpvReflectShaderModule reflect_vertex_shader_module = {0};
  SpvReflectShaderModule reflect_fragment_shader_module = {0};

  uint8_t *vertex_shader_bytes = 0;
  uint8_t *fragment_shader_bytes = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesys_load_binary(&vertex_shader_bytes, &vertex_shader_size, vertex_shader_file_path);
  filesys_load_binary(&fragment_shader_bytes, &fragment_shader_size, fragment_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(vertex_shader_size, vertex_shader_bytes, &reflect_vertex_shader_module));
  SPIRV_CHECK(spvReflectCreateShaderModule(fragment_shader_size, fragment_shader_bytes, &reflect_fragment_shader_module));

  pipeline_id_t pipeline_id = database_store_pipeline_asset(pipeline_name, 0, 0, 1);

  database_store_pipeline_resource(pipeline_id, vertex_shader_file_path, fragment_shader_file_path, 0, vertex_shader_bytes, fragment_shader_bytes, 0, vertex_shader_size, fragment_shader_size, 0);

  importer_process_vertex_input_bindings(pipeline_id, &reflect_vertex_shader_module);

  importer_process_descriptor_bindings(pipeline_id, &reflect_vertex_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  importer_process_descriptor_bindings(pipeline_id, &reflect_fragment_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

  spvReflectDestroyShaderModule(&reflect_vertex_shader_module);
  spvReflectDestroyShaderModule(&reflect_fragment_shader_module);

  heap_free(vertex_shader_bytes);
  heap_free(fragment_shader_bytes);
}
void importer_import_compute_pipeline(char const *pipeline_name, char const *compute_shader_file_path) {
  SpvReflectShaderModule reflect_compute_shader_module = {0};

  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesys_load_binary(&compute_shader_bytes, &compute_shader_size, compute_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(compute_shader_size, compute_shader_bytes, &reflect_compute_shader_module));

  pipeline_id_t pipeline_id = database_store_pipeline_asset(pipeline_name, 1, 0, 1);

  database_store_pipeline_resource(pipeline_id, 0, 0, compute_shader_file_path, 0, 0, compute_shader_bytes, 0, 0, compute_shader_size);

  importer_process_descriptor_bindings(pipeline_id, &reflect_compute_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

  spvReflectDestroyShaderModule(&reflect_compute_shader_module);

  heap_free(compute_shader_bytes);
}

void importer_import_model(char const *model_name, char const *model_file_path) {
  uint8_t *model_bytes = 0;

  uint64_t model_size = 0;

  filesys_load_binary(&model_bytes, &model_size, model_file_path);

  cgltf_options gltf_options = {0};

  cgltf_data *gltf_data = 0;

  CGLTF_CHECK(cgltf_parse(&gltf_options, model_bytes, model_size, &gltf_data));

  /*
  CGLTF_CHECK(cgltf_load_buffers(&options, data, filename));

  cgltf_mesh *mesh = &data->meshes[0];
  cgltf_primitive *prim = &mesh->primitives[0];
  cgltf_accessor *pos_acc = NULL;
  for (size_t i = 0; i < prim->attributes_count; ++i) {
    if (prim->attributes[i].type == cgltf_attribute_type_position) {
      pos_acc = prim->attributes[i].data;
      break;
    }
  }

  size_t vertex_count = pos_acc->count;
    float* positions = malloc(sizeof(float) * 3 * vertex_count);
    for (size_t v = 0; v < vertex_count; ++v)
    {
        cgltf_accessor_read_float(pos_acc, v, &positions[v*3], 3);
        // positions[v*3 + 0..2] now contain x,y,z
    }

        uint32_t* indices = NULL;
    size_t index_count = 0;
    if (prim->indices)
    {
        cgltf_accessor* idx_acc = prim->indices;
        index_count = idx_acc->count;
        indices = malloc(sizeof(uint32_t) * index_count);
        for (size_t i = 0; i < index_count; ++i)
        {
            uint32_t v;
            cgltf_accessor_read_uint(idx_acc, i, &v, 1);
            indices[i] = v;
        }
    }
  */

  cgltf_free(gltf_data);

  heap_free(model_bytes);
}

static void importer_process_vertex_input_bindings(pipeline_id_t pipeline_id, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint64_t input_variable_index = 0;
  uint32_t byte_offset = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    database_store_pipeline_vertex_input_binding(pipeline_id, input_variable->name, 0, input_variable->location, 0, input_variable->format, byte_offset, VK_VERTEX_INPUT_RATE_VERTEX);

    byte_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void importer_process_descriptor_bindings(pipeline_id_t pipeline_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    database_store_pipeline_descriptor_binding(pipeline_id, descriptor_binding->name, descriptor_binding->binding, descriptor_binding->descriptor_type, 1, stage_flag, 1);

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}
