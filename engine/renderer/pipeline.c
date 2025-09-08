#include <stdio.h>
#include <string.h>

#include <vulkan/vulkan.h>

#include <engine/macros.h>

#include <engine/renderer/pipeline.h>

pipeline_t pipeline_alloc(char const *pipeline_name) {
  pipeline_t pipeline = {0};

  strcpy(pipeline.pipeline_name, pipeline_name);
  pipeline.vertex_input_binding_descriptions = vector_alloc(sizeof(VkVertexInputBindingDescription));
  pipeline.vertex_input_attribute_descriptions = vector_alloc(sizeof(VkVertexInputAttributeDescription));

  return pipeline;
}
void pipeline_print(pipeline_t *pipeline) {
  printf("Pipeline: %s\n", pipeline->pipeline_name);

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
}
void pipeline_free(pipeline_t *pipeline) {
  vector_free(&pipeline->vertex_input_binding_descriptions);
  vector_free(&pipeline->vertex_input_attribute_descriptions);
}

void pipeline_push_input_binding_descriptions(pipeline_t *pipeline, SpvReflectShaderModule *shader_module) {
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

    vector_push(&pipeline->vertex_input_binding_descriptions, &vertex_input_binding_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}
void pipeline_push_input_attribute_descriptions(pipeline_t *pipeline, SpvReflectShaderModule *shader_module) {
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

    vector_push(&pipeline->vertex_input_attribute_descriptions, &vertex_input_attribute_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}
