#include <library/lb_pch.h>

#include <library/database/lb_api.h>

#include <library/importer/lb_importer.h>

#include <spirv_reflect/spirv_reflect.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

static void importer_process_pipeline_vertex_input_bindings(pipeline_asset_t *pipeline_asset, SpvReflectShaderModule *shader_module);
static void importer_process_pipeline_descriptor_bindings(pipeline_asset_t *pipeline_asset, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag);

void importer_import_default_assets(void) {
  graphic_pipeline_import_settings_t debug_line_import_settings = {0};

  debug_line_import_settings.pipeline_name = "debug_line";
  debug_line_import_settings.vertex_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\debug\\line.vert.spv";
  debug_line_import_settings.fragment_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\debug\\line.frag.spv";
  debug_line_import_settings.auto_create_pipeline = 1;
  debug_line_import_settings.auto_create_vertex_input_buffer = 0; // TODO: experimental..
  debug_line_import_settings.auto_link_descriptor_bindings = 1;
  debug_line_import_settings.interleaved_vertex_input = 1;

  importer_import_graphic_pipeline(&debug_line_import_settings);

  graphic_pipeline_import_settings_t principled_brdf_import_settings = {0};

  principled_brdf_import_settings.pipeline_name = "principled_brdf";
  principled_brdf_import_settings.vertex_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\principled\\brdf.vert.spv";
  principled_brdf_import_settings.fragment_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\principled\\brdf.frag.spv";
  principled_brdf_import_settings.auto_create_pipeline = 1;
  principled_brdf_import_settings.auto_create_vertex_input_buffer = 0; // TODO: experimental..
  principled_brdf_import_settings.auto_link_descriptor_bindings = 1;
  principled_brdf_import_settings.interleaved_vertex_input = 1;

  importer_import_graphic_pipeline(&principled_brdf_import_settings);

  graphic_pipeline_import_settings_t terrain_import_settings = {0};

  terrain_import_settings.pipeline_name = "terrain";
  terrain_import_settings.vertex_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\terrain\\terrain.vert.spv";
  terrain_import_settings.fragment_shader_file_path = "C:\\Users\\burm\\Downloads\\fuse\\shader\\terrain\\terrain.frag.spv";
  terrain_import_settings.auto_create_pipeline = 0;
  terrain_import_settings.auto_create_vertex_input_buffer = 0; // TODO: experimental..
  terrain_import_settings.auto_link_descriptor_bindings = 1;
  terrain_import_settings.interleaved_vertex_input = 1;

  importer_import_graphic_pipeline(&terrain_import_settings);
}

void importer_import_graphic_pipeline(graphic_pipeline_import_settings_t *import_settings) {
  SpvReflectShaderModule reflect_vertex_shader_module = {0};
  SpvReflectShaderModule reflect_fragment_shader_module = {0};

  uint8_t *vertex_shader_bytes = 0;
  uint8_t *fragment_shader_bytes = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesys_load_binary(&vertex_shader_bytes, &vertex_shader_size, import_settings->vertex_shader_file_path);
  filesys_load_binary(&fragment_shader_bytes, &fragment_shader_size, import_settings->fragment_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(vertex_shader_size, vertex_shader_bytes, &reflect_vertex_shader_module));
  SPIRV_CHECK(spvReflectCreateShaderModule(fragment_shader_size, fragment_shader_bytes, &reflect_fragment_shader_module));

  pipeline_asset_t pipeline_asset = {0};

  pipeline_asset.name = import_settings->pipeline_name;
  pipeline_asset.type = 0;
  pipeline_asset.link_index = 0;
  pipeline_asset.auto_create_pipeline = import_settings->auto_create_pipeline;
  pipeline_asset.auto_create_vertex_input_buffer = import_settings->auto_create_vertex_input_buffer;
  pipeline_asset.auto_link_descriptor_bindings = import_settings->auto_link_descriptor_bindings;
  pipeline_asset.interleaved_vertex_input_buffer = import_settings->interleaved_vertex_input;

  database_store_pipeline_asset(&pipeline_asset);

  pipeline_resource_t pipeline_resource = {0};

  pipeline_resource.pipeline_asset_id = pipeline_asset.id;
  pipeline_resource.vertex_shader_file_path = import_settings->vertex_shader_file_path;
  pipeline_resource.fragment_shader_file_path = import_settings->fragment_shader_file_path;

  database_store_pipeline_resource(&pipeline_resource);

  importer_process_pipeline_vertex_input_bindings(&pipeline_asset, &reflect_vertex_shader_module);

  importer_process_pipeline_descriptor_bindings(&pipeline_asset, &reflect_vertex_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  importer_process_pipeline_descriptor_bindings(&pipeline_asset, &reflect_fragment_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

  spvReflectDestroyShaderModule(&reflect_vertex_shader_module);
  spvReflectDestroyShaderModule(&reflect_fragment_shader_module);

  heap_free(vertex_shader_bytes);
  heap_free(fragment_shader_bytes);
}
void importer_import_compute_pipeline(compute_pipeline_import_settings_t *import_settings) {
  SpvReflectShaderModule reflect_compute_shader_module = {0};

  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesys_load_binary(&compute_shader_bytes, &compute_shader_size, import_settings->compute_shader_file_path);

  SPIRV_CHECK(spvReflectCreateShaderModule(compute_shader_size, compute_shader_bytes, &reflect_compute_shader_module));

  pipeline_asset_t pipeline_asset = {0};

  pipeline_asset.name = import_settings->pipeline_name;
  pipeline_asset.type = 1;
  pipeline_asset.link_index = 0;
  pipeline_asset.auto_create_pipeline = 1;
  pipeline_asset.auto_create_vertex_input_buffer = 0;
  pipeline_asset.auto_link_descriptor_bindings = 1;
  pipeline_asset.interleaved_vertex_input_buffer = 0;

  database_store_pipeline_asset(&pipeline_asset);

  pipeline_resource_t pipeline_resource = {0};

  pipeline_resource.pipeline_asset_id = pipeline_asset.id;
  pipeline_resource.compute_shader_file_path = import_settings->compute_shader_file_path;

  database_store_pipeline_resource(&pipeline_resource);

  importer_process_pipeline_descriptor_bindings(&pipeline_asset, &reflect_compute_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

  spvReflectDestroyShaderModule(&reflect_compute_shader_module);

  heap_free(compute_shader_bytes);
}

void importer_import_model(model_import_settings_t *import_settings) {
  uint8_t *model_bytes = 0;

  uint64_t model_size = 0;

  filesys_load_binary(&model_bytes, &model_size, import_settings->model_file_path);

  cgltf_options gltf_options = {0};

  cgltf_data *gltf_data = 0;

  CGLTF_CHECK(cgltf_parse(&gltf_options, model_bytes, model_size, &gltf_data));
  CGLTF_CHECK(cgltf_load_buffers(&gltf_options, gltf_data, import_settings->model_file_path));

  model_asset_t model_asset = {0};

  model_asset.name = import_settings->model_name;

  database_store_model_asset(&model_asset);

  model_resource_t model_resource = {0};

  model_resource.model_asset_id = model_asset.id;
  model_resource.model_file_path = import_settings->model_file_path;

  database_store_model_resource(&model_resource);

  uint64_t mesh_index = 0;
  uint64_t mesh_count = gltf_data->meshes_count;

  while (mesh_index < mesh_count) {
    cgltf_mesh const *gltf_mesh = &gltf_data->meshes[mesh_index];

    model_mesh_t model_mesh = {0};

    model_mesh.model_asset_id = model_asset.id;

    database_store_model_mesh(&model_mesh);

    uint64_t prim_index = 0;
    uint64_t prim_count = gltf_mesh->primitives_count;

    while (prim_index < prim_count) {
      cgltf_primitive const *gltf_prim = &gltf_mesh->primitives[prim_index];

      model_primitive_t model_primitive = {0};

      model_primitive.model_mesh_id = model_mesh.id;

      database_store_model_primitive(&model_primitive);

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
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_vec2: {
            value_size = 2 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_vec3: {
            value_size = 3 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_vec4: {
            value_size = 4 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_mat2: {
            value_size = 4 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_mat3: {
            value_size = 9 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
            break;
          }
          case cgltf_type_mat4: {
            value_size = 16 * gltf_accessor->count;
            value_bytes = (uint8_t *)heap_alloc(sizeof(float) * value_size, 0, 0);
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

        model_attribute_t model_attribute = {0};

        model_attribute.model_primitive_id = model_primitive.id;
        model_attribute.name = gltf_attrib->name;
        model_attribute.type = gltf_accessor->type;

        database_store_model_attribute(&model_attribute);

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

static void importer_process_pipeline_vertex_input_bindings(pipeline_asset_t *pipeline_asset, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count, 0, 0);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint64_t input_variable_index = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    pipeline_vertex_input_binding.pipeline_asset_id = pipeline_asset->id;
    pipeline_vertex_input_binding.binding_name = (char *)input_variable->name;
    pipeline_vertex_input_binding.location = input_variable->location;
    pipeline_vertex_input_binding.size = input_variable->numeric.scalar.width / 8;
    pipeline_vertex_input_binding.component_count = input_variable->numeric.vector.component_count;
    pipeline_vertex_input_binding.format = input_variable->format;
    pipeline_vertex_input_binding.input_rate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO

    database_store_pipeline_vertex_input_binding(&pipeline_vertex_input_binding);

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void importer_process_pipeline_descriptor_bindings(pipeline_asset_t *pipeline_asset, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count, 0, 0);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    pipeline_descriptor_binding_t pipeline_descriptor_binding = {0};

    pipeline_descriptor_binding.pipeline_asset_id = pipeline_asset->id;
    pipeline_descriptor_binding.binding_name = (char *)descriptor_binding->name;
    pipeline_descriptor_binding.binding = descriptor_binding->binding;
    pipeline_descriptor_binding.descriptor_type = descriptor_binding->descriptor_type;
    pipeline_descriptor_binding.descriptor_count = 1;
    pipeline_descriptor_binding.stage_flags = stage_flag;
    pipeline_descriptor_binding.auto_buffer = 1;

    database_store_pipeline_descriptor_binding(&pipeline_descriptor_binding);

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}
