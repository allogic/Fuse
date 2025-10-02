#include <library/lb_pch.h>

#include <library/core/co_api.h>
#include <library/database/db_api.h>

#include <spirv_reflect/spirv_reflect.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

static void importer_process_pipeline_vertex_input_bindings(pipeline_asset_id_t pipeline_asset_id, SpvReflectShaderModule *shader_module);
static void importer_process_pipeline_descriptor_bindings(pipeline_asset_id_t pipeline_asset_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag);

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

  pipeline_asset_id_t pipeline_asset_id = database_store_pipeline_asset(pipeline_name, 0, 0, 1);

  database_store_pipeline_resource(pipeline_asset_id, vertex_shader_file_path, fragment_shader_file_path, 0, vertex_shader_bytes, fragment_shader_bytes, 0, vertex_shader_size, fragment_shader_size, 0);

  importer_process_pipeline_vertex_input_bindings(pipeline_asset_id, &reflect_vertex_shader_module);

  importer_process_pipeline_descriptor_bindings(pipeline_asset_id, &reflect_vertex_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  importer_process_pipeline_descriptor_bindings(pipeline_asset_id, &reflect_fragment_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

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

  pipeline_asset_id_t pipeline_asset_id = database_store_pipeline_asset(pipeline_name, 1, 0, 1);

  database_store_pipeline_resource(pipeline_asset_id, 0, 0, compute_shader_file_path, 0, 0, compute_shader_bytes, 0, 0, compute_shader_size);

  importer_process_pipeline_descriptor_bindings(pipeline_asset_id, &reflect_compute_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

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
  CGLTF_CHECK(cgltf_load_buffers(&gltf_options, gltf_data, model_file_path));

  model_asset_id_t model_asset_id = database_store_model_asset(model_name);

  database_store_model_resource(model_asset_id, model_file_path, model_bytes, model_size);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = gltf_data->meshes_count;

  while (mesh_index < mesh_count) {
    cgltf_mesh const *gltf_mesh = &gltf_data->meshes[mesh_index];

    model_mesh_id_t model_mesh_id = database_store_model_mesh(model_asset_id);

    uint64_t prim_index = 0;
    uint64_t prim_count = gltf_mesh->primitives_count;

    while (prim_index < prim_count) {
      cgltf_primitive const *gltf_prim = &gltf_mesh->primitives[prim_index];

      model_primitive_id_t model_primitive_id = database_store_model_primitive(model_mesh_id);

      uint64_t attrib_index = 0;
      uint64_t attrib_count = gltf_prim->attributes_count;

      while (attrib_index < attrib_count) {
        cgltf_attribute const *gltf_attrib = &gltf_prim->attributes[attrib_index];

        cgltf_accessor const *gltf_accessor = gltf_attrib->data;

        uint8_t *value_bytes = 0;
        uint64_t value_size = 0;

        switch (gltf_accessor->type) {
          case cgltf_type_scalar: {
            value_size = gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_vec2: {
            value_size = 2 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_vec3: {
            value_size = 3 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_vec4: {
            value_size = 4 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_mat2: {
            value_size = 4 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_mat3: {
            value_size = 9 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
          case cgltf_type_mat4: {
            value_size = 16 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size);
            break;
          }
        }

        uint64_t value_index = 0;
        uint64_t value_count = gltf_accessor->count;

        while (value_index < value_count) {

          // TODO: also support integers..

          switch (gltf_accessor->type) {
            case cgltf_type_scalar: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index], 1);
              break;
            }
            case cgltf_type_vec2: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 2], 2);
              break;
            }
            case cgltf_type_vec3: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 3], 3);
              break;
            }
            case cgltf_type_vec4: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 4], 4);
              break;
            }
            case cgltf_type_mat2: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 4], 4);
              break;
            }
            case cgltf_type_mat3: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 9], 9);
              break;
            }
            case cgltf_type_mat4: {
              cgltf_accessor_read_float(gltf_accessor, value_index, (float *)&value_bytes[value_index * 16], 16);
              break;
            }
          }

          value_index++;
        }

        database_store_model_attribute(model_primitive_id, gltf_attrib->name, (uint32_t)gltf_accessor->type, value_bytes, value_size);

        heap_free(value_bytes);

        attrib_index++;
      }

      prim_index++;
    }

    mesh_index++;
  }

  // uint32_t *indices = NULL;
  // size_t index_count = 0;
  // if (prim->indices) {
  //   cgltf_accessor *idx_acc = prim->indices;
  //   index_count = idx_acc->count;
  //   indices = malloc(sizeof(uint32_t) * index_count);
  //   for (size_t i = 0; i < index_count; ++i) {
  //     uint32_t v;
  //     cgltf_accessor_read_uint(idx_acc, i, &v, 1);
  //     indices[i] = v;
  //   }
  // }

  cgltf_free(gltf_data);

  heap_free(model_bytes);
}

static void importer_process_pipeline_vertex_input_bindings(pipeline_asset_id_t pipeline_asset_id, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint64_t input_variable_index = 0;
  uint32_t byte_offset = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    database_store_pipeline_vertex_input_binding(pipeline_asset_id, input_variable->name, 0, input_variable->location, 0, input_variable->format, byte_offset, VK_VERTEX_INPUT_RATE_VERTEX);

    byte_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void importer_process_pipeline_descriptor_bindings(pipeline_asset_id_t pipeline_asset_id, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    database_store_pipeline_descriptor_binding(pipeline_asset_id, descriptor_binding->name, descriptor_binding->binding, descriptor_binding->descriptor_type, 1, stage_flag, 1);

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}
