#include <stdio.h>
#include <string.h>

#include <engine/macros.h>
#include <engine/context.h>

#include <engine/renderer/pipeline.h>

static void pipeline_create_descriptor_pools(vector_t *descriptor_pool_sizes);

graphic_pipeline_t graphic_pipeline_alloc(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path) {
  graphic_pipeline_t pipeline = {0};

  strcpy(pipeline.pipeline_name, pipeline_name);

  pipeline.vertex_input_binding_descriptions = vector_alloc(sizeof(VkVertexInputBindingDescription));
  pipeline.vertex_input_attribute_descriptions = vector_alloc(sizeof(VkVertexInputAttributeDescription));
  pipeline.descriptor_pool_sizes = vector_alloc(sizeof(VkDescriptorPoolSize));
  pipeline.descriptor_set_layout_binding = vector_alloc(sizeof(VkDescriptorSetLayoutBinding));

  uint8_t *vertex_buffer = 0;
  uint8_t *fragment_buffer = 0;

  uint64_t vertex_buffer_size = 0;
  uint64_t fragment_buffer_size = 0;

  filesys_load_binary(&vertex_buffer, &vertex_buffer_size, vertex_shader_file_path);
  filesys_load_binary(&fragment_buffer, &fragment_buffer_size, fragment_shader_file_path);

  SpvReflectShaderModule vertex_shader_module = {0};
  SpvReflectShaderModule fragment_shader_module = {0};

  SPIRV_CHECK(spvReflectCreateShaderModule(vertex_buffer_size, vertex_buffer, &vertex_shader_module));
  SPIRV_CHECK(spvReflectCreateShaderModule(fragment_buffer_size, fragment_buffer, &fragment_shader_module));

  pipeline_push_input_binding_descriptions(&pipeline.vertex_input_binding_descriptions, &vertex_shader_module);
  pipeline_push_input_attribute_descriptions(&pipeline.vertex_input_attribute_descriptions, &vertex_shader_module);
  pipeline_push_descriptor_bindings(&pipeline.descriptor_pool_sizes, &vertex_shader_module);

  spvReflectDestroyShaderModule(&vertex_shader_module);
  spvReflectDestroyShaderModule(&fragment_shader_module);

  heap_free(vertex_buffer);
  heap_free(fragment_buffer);

  return pipeline;
}
void graphic_pipeline_print(graphic_pipeline_t *pipeline) {
  printf("GraphicPipeline: %s\n", pipeline->pipeline_name);

  printf("\tVertexInputBindingDescriptions\n");

  uint64_t vertex_input_binding_description_index = 0;
  uint64_t vertex_input_binding_description_count = vector_count(&pipeline->vertex_input_binding_descriptions);

  while (vertex_input_binding_description_index < vertex_input_binding_description_count) {
    VkVertexInputBindingDescription *vertex_input_binding_description = (VkVertexInputBindingDescription *)vector_at(&pipeline->vertex_input_binding_descriptions, vertex_input_binding_description_index);

    printf("\t\t%llu:\n", vertex_input_binding_description_index);
    printf("\t\t\tBinding: %u\n", vertex_input_binding_description->binding);
    printf("\t\t\tStride: %u\n", vertex_input_binding_description->stride);
    printf("\t\t\tInputRate: %u\n", (uint32_t)vertex_input_binding_description->inputRate);

    vertex_input_binding_description_index++;
  }

  printf("\tVertexInputAttributeDescriptions\n");

  uint64_t vertex_input_attribute_description_index = 0;
  uint64_t vertex_input_attribute_description_count = vector_count(&pipeline->vertex_input_attribute_descriptions);

  while (vertex_input_attribute_description_index < vertex_input_attribute_description_count) {
    VkVertexInputAttributeDescription *vertex_input_attribute_description = (VkVertexInputAttributeDescription *)vector_at(&pipeline->vertex_input_attribute_descriptions, vertex_input_attribute_description_index);

    printf("\t\t%llu:\n", vertex_input_attribute_description_index);
    printf("\t\t\tLocation: %u\n", vertex_input_attribute_description->location);
    printf("\t\t\tBinding: %u\n", vertex_input_attribute_description->binding);
    printf("\t\t\tFormat: %u\n", vertex_input_attribute_description->format);
    printf("\t\t\tOffset: %u\n", vertex_input_attribute_description->offset);

    vertex_input_attribute_description_index++;
  }

  printf("\tDescriptorPoolSizes\n");

  uint64_t descriptor_pool_size_index = 0;
  uint64_t descriptor_pool_size_count = vector_count(&pipeline->descriptor_pool_sizes);

  while (descriptor_pool_size_index < descriptor_pool_size_count) {
    VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)vector_at(&pipeline->descriptor_pool_sizes, descriptor_pool_size_index);

    printf("\t\t%llu:\n", descriptor_pool_size_index);
    printf("\t\t\tType: %u\n", (uint32_t)descriptor_pool_size->type);
    printf("\t\t\tDescriptorCount: %u\n", descriptor_pool_size->descriptorCount);

    descriptor_pool_size_index++;
  }
}
void graphic_pipeline_free(graphic_pipeline_t *pipeline) {
  vector_free(&pipeline->vertex_input_binding_descriptions);
  vector_free(&pipeline->vertex_input_attribute_descriptions);
  vector_free(&pipeline->descriptor_pool_sizes);
  vector_free(&pipeline->descriptor_set_layout_binding);
}

compute_pipeline_t compute_pipeline_alloc(char const *pipeline_name, char const *compute_shader_file_path) {
  compute_pipeline_t pipeline = {0};

  strcpy(pipeline.pipeline_name, pipeline_name);

  pipeline.descriptor_pool_sizes = vector_alloc(sizeof(VkDescriptorPoolSize));
  pipeline.descriptor_set_layout_binding = vector_alloc(sizeof(VkDescriptorSetLayoutBinding));

  uint8_t *compute_buffer = 0;

  uint64_t compute_buffer_size = 0;

  filesys_load_binary(&compute_buffer, &compute_buffer_size, compute_shader_file_path);

  SpvReflectShaderModule compute_shader_module = {0};

  SPIRV_CHECK(spvReflectCreateShaderModule(compute_buffer_size, compute_buffer, &compute_shader_module));

  pipeline_push_descriptor_bindings(&pipeline.descriptor_pool_sizes, &compute_shader_module);

  spvReflectDestroyShaderModule(&compute_shader_module);

  heap_free(compute_buffer);

  return pipeline;
}
void compute_pipeline_print(compute_pipeline_t *pipeline) {
  printf("ComputePipeline: %s\n", pipeline->pipeline_name);

  printf("\tDescriptorPoolSizes\n");

  uint64_t descriptor_pool_size_index = 0;
  uint64_t descriptor_pool_size_count = vector_count(&pipeline->descriptor_pool_sizes);

  while (descriptor_pool_size_index < descriptor_pool_size_count) {
    VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)vector_at(&pipeline->descriptor_pool_sizes, descriptor_pool_size_index);

    printf("\t\t%llu:\n", descriptor_pool_size_index);
    printf("\t\t\tType: %u\n", (uint32_t)descriptor_pool_size->type);
    printf("\t\t\tDescriptorCount: %u\n", descriptor_pool_size->descriptorCount);

    descriptor_pool_size_index++;
  }
}
void compute_pipeline_free(compute_pipeline_t *pipeline) {
  vector_free(&pipeline->descriptor_pool_sizes);
  vector_free(&pipeline->descriptor_set_layout_binding);
}

void pipeline_push_input_binding_descriptions(vector_t *vertex_input_binding_descriptions, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint32_t input_variable_index = 0;
  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    VkVertexInputBindingDescription vertex_input_binding_description = {0};

    vertex_input_binding_description.binding = 0;
    vertex_input_binding_description.stride = 0;                              // TODO sizeof(vertex_t)
    vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO

    vector_push(vertex_input_binding_descriptions, &vertex_input_binding_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}
void pipeline_push_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint32_t current_offset = 0;

  uint32_t input_variable_index = 0;
  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    VkVertexInputAttributeDescription vertex_input_attribute_description = {0};

    vertex_input_attribute_description.location = input_variable->location;
    vertex_input_attribute_description.binding = 0;
    vertex_input_attribute_description.format = (VkFormat)input_variable->format;
    vertex_input_attribute_description.offset = current_offset;

    current_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    vector_push(vertex_input_attribute_descriptions, &vertex_input_attribute_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}
void pipeline_push_descriptor_bindings(vector_t *descriptor_pool_sizes, SpvReflectShaderModule *shader_module) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    VkDescriptorType descriptor_type = (VkDescriptorType)descriptor_binding->descriptor_type;

    uint8_t descriptor_pool_size_exists = 0;

    uint64_t descriptor_pool_index = 0;
    uint64_t descriptor_pool_count = vector_count(descriptor_pool_sizes);
    while (descriptor_pool_index < descriptor_pool_count) {
      VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)vector_at(descriptor_pool_sizes, descriptor_pool_index);

      if (descriptor_pool_size->type == descriptor_type) {
        descriptor_pool_size_exists = 1;

        break;
      }

      descriptor_pool_index++;
    }

    if (descriptor_pool_size_exists) {
      VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)vector_at(descriptor_pool_sizes, descriptor_pool_index);

      descriptor_pool_size->descriptorCount++;
    } else {
      VkDescriptorPoolSize descriptor_pool_size = {0};

      descriptor_pool_size.type = descriptor_type;
      descriptor_pool_size.descriptorCount = 1;

      vector_push(descriptor_pool_sizes, &descriptor_pool_size);
    }

    descriptor_binding_index++;
  }
}

static void pipeline_create_descriptor_pools(vector_t *descriptor_pool_sizes) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &descriptor_pool_create_info, 0, &s_renderer_chunk_editor_descriptor_pool));
}
static void renderer_create_descriptor_set_layouts(void) {
  VkDescriptorSetLayoutBinding chunk_editor_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_editor_descriptor_set_layout_create_info = {0};
  chunk_editor_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_editor_descriptor_set_layout_create_info.pBindings = chunk_editor_descriptor_set_layout_bindings;
  chunk_editor_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_editor_descriptor_set_layout_bindings);
  chunk_editor_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_editor_descriptor_set_layout_create_info, 0, &s_renderer_chunk_editor_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_generator_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_generator_descriptor_set_layout_create_info = {0};
  chunk_generator_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_generator_descriptor_set_layout_create_info.pBindings = chunk_generator_descriptor_set_layout_bindings;
  chunk_generator_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_generator_descriptor_set_layout_bindings);
  chunk_generator_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_generator_descriptor_set_layout_create_info, 0, &s_renderer_chunk_generator_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_mipmap_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_mipmap_descriptor_set_layout_create_info = {0};
  chunk_mipmap_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_mipmap_descriptor_set_layout_create_info.pBindings = chunk_mipmap_descriptor_set_layout_bindings;
  chunk_mipmap_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_mipmap_descriptor_set_layout_bindings);
  chunk_mipmap_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_mipmap_descriptor_set_layout_create_info, 0, &s_renderer_chunk_mipmap_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_renderer_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_renderer_descriptor_set_layout_create_info = {0};
  chunk_renderer_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_renderer_descriptor_set_layout_create_info.pBindings = chunk_renderer_descriptor_set_layout_bindings;
  chunk_renderer_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_renderer_descriptor_set_layout_bindings);
  chunk_renderer_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_renderer_descriptor_set_layout_create_info, 0, &s_renderer_chunk_renderer_descriptor_set_layout));

  VkDescriptorSetLayoutBinding debug_line_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo debug_line_descriptor_set_layout_create_info = {0};
  debug_line_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  debug_line_descriptor_set_layout_create_info.pBindings = debug_line_descriptor_set_layout_bindings;
  debug_line_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(debug_line_descriptor_set_layout_bindings);
  debug_line_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &debug_line_descriptor_set_layout_create_info, 0, &s_renderer_debug_line_descriptor_set_layout));
}
