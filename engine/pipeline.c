#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <spirv_reflect/spirv_reflect.h>

#include <library/core/api.h>

#include <engine/context.h>
#include <engine/database.h>
#include <engine/macros.h>
#include <engine/pipeline.h>
#include <engine/swapchain.h>

static uint32_t pipeline_descriptor_name_to_binding(map_t *descriptor_name_to_binding, char const *binding_name);

static void pipeline_insert_descriptor_binding_names(map_t *descriptor_name_to_binding, SpvReflectShaderModule *shader_module);

static void pipeline_print_descriptor_binding_names(map_t *descriptor_name_to_binding);

static void pipeline_push_vertex_input_binding_description(vector_t *vertex_input_binding_descriptions, SpvReflectShaderModule *shader_module);
static void pipeline_push_vertex_input_attribute_description(vector_t *vertex_input_attribute_descriptions, SpvReflectShaderModule *shader_module);

static void pipeline_print_vertex_input_binding_descriptions(vector_t *vertex_input_binding_descriptions);
static void pipeline_print_vertex_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions);

static void pipeline_insert_descriptor_pool_size(map_t *descriptor_pool_sizes, SpvReflectShaderModule *shader_module);
static void pipeline_insert_descriptor_set_layout_binding(map_t *descriptor_set_layout_bindings, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag);

static void pipeline_print_descriptor_pool_sizes(vector_t *descriptor_pool_sizes);
static void pipeline_print_descriptor_set_layout_bindings(vector_t *descriptor_set_layout_bindings);

static void pipeline_flatten_descriptor_pool_sizes(map_t *descriptor_pool_size_mappings, vector_t *descriptor_pool_sizes);
static void pipeline_flatten_descriptor_set_layout_bindings(map_t *descriptor_set_layout_binding_mappings, vector_t *descriptor_set_layout_bindings);

graphic_pipeline_t graphic_pipeline_create(int32_t frames_in_flight, string_t *pipeline_name) {
  graphic_pipeline_t pipeline = {0};

  database_graphic_pipeline_settings_t db_settings = database_load_graphic_pipeline_settings_by_name(string_buffer(pipeline_name));

  pipeline.frames_in_flight = frames_in_flight;
  pipeline.vertex_input_binding_descriptions = vector_create(sizeof(VkVertexInputBindingDescription));
  pipeline.vertex_input_attribute_descriptions = vector_create(sizeof(VkVertexInputAttributeDescription));
  pipeline.descriptor_pool_sizes = vector_create(sizeof(VkDescriptorPoolSize));
  pipeline.descriptor_set_layout_bindings = vector_create(sizeof(VkDescriptorSetLayoutBinding));
  pipeline.descriptor_set_layouts = vector_create(sizeof(VkDescriptorSetLayout));
  pipeline.descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline.write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));
  pipeline.descriptor_name_to_binding = map_create();
  pipeline.buffer_mappings = map_create();

  map_t descriptor_pool_size_mappings = map_create();
  map_t descriptor_set_layout_binding_mappings = map_create();

  SpvReflectShaderModule reflect_vertex_shader_module = {0};
  SpvReflectShaderModule reflect_fragment_shader_module = {0};

  VkShaderModule vertex_shader_module = 0;
  VkShaderModule fragment_shader_module = 0;

  SPIRV_CHECK(spvReflectCreateShaderModule(vector_size(&db_settings.vertex_shader), vector_buffer(&db_settings.vertex_shader), &reflect_vertex_shader_module));
  SPIRV_CHECK(spvReflectCreateShaderModule(vector_size(&db_settings.fragment_shader), vector_buffer(&db_settings.fragment_shader), &reflect_fragment_shader_module));

  pipeline_push_vertex_input_binding_description(&pipeline.vertex_input_binding_descriptions, &reflect_vertex_shader_module);
  pipeline_push_vertex_input_attribute_description(&pipeline.vertex_input_attribute_descriptions, &reflect_vertex_shader_module);

  pipeline_insert_descriptor_pool_size(&descriptor_pool_size_mappings, &reflect_vertex_shader_module);
  pipeline_insert_descriptor_pool_size(&descriptor_pool_size_mappings, &reflect_fragment_shader_module);

  pipeline_insert_descriptor_set_layout_binding(&descriptor_set_layout_binding_mappings, &reflect_vertex_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
  pipeline_insert_descriptor_set_layout_binding(&descriptor_set_layout_binding_mappings, &reflect_fragment_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

  pipeline_flatten_descriptor_pool_sizes(&descriptor_pool_size_mappings, &pipeline.descriptor_pool_sizes);
  pipeline_flatten_descriptor_set_layout_bindings(&descriptor_set_layout_binding_mappings, &pipeline.descriptor_set_layout_bindings);

  pipeline_insert_descriptor_binding_names(&pipeline.descriptor_name_to_binding, &reflect_vertex_shader_module);
  pipeline_insert_descriptor_binding_names(&pipeline.descriptor_name_to_binding, &reflect_fragment_shader_module);

#ifdef BUILD_DEBUG
  printf("%s\n", string_buffer(&db_settings.pipeline_name));
  pipeline_print_vertex_input_binding_descriptions(&pipeline.vertex_input_binding_descriptions);
  pipeline_print_vertex_input_attribute_descriptions(&pipeline.vertex_input_attribute_descriptions);
  pipeline_print_descriptor_pool_sizes(&pipeline.descriptor_pool_sizes);
  pipeline_print_descriptor_set_layout_bindings(&pipeline.descriptor_set_layout_bindings);
  pipeline_print_descriptor_binding_names(&pipeline.descriptor_name_to_binding);
  printf("\n");
#endif // BUILD_DEBUG

  map_destroy(&descriptor_pool_size_mappings);
  map_destroy(&descriptor_set_layout_binding_mappings);

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&pipeline.descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&pipeline.descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &descriptor_pool_create_info, 0, &pipeline.descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&pipeline.descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&pipeline.descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &descriptor_set_layout_create_info, 0, &pipeline.descriptor_set_layout));

  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &pipeline.descriptor_set_layout;
  pipeline_layout_create_info.pPushConstantRanges = 0;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &pipeline_layout_create_info, 0, &pipeline.pipeline_layout));

  VkShaderModuleCreateInfo vertex_shader_module_create_info = {0};
  vertex_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertex_shader_module_create_info.codeSize = vector_size(&db_settings.vertex_shader);
  vertex_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&db_settings.vertex_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_context_device, &vertex_shader_module_create_info, 0, &vertex_shader_module));

  VkShaderModuleCreateInfo fragment_shader_module_create_info = {0};
  fragment_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragment_shader_module_create_info.codeSize = vector_size(&db_settings.fragment_shader);
  fragment_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&db_settings.fragment_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_context_device, &fragment_shader_module_create_info, 0, &fragment_shader_module));

  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {0};
  vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_stage_create_info.module = vertex_shader_module;
  vertex_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {0};
  fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_shader_stage_create_info.module = fragment_shader_module;
  fragment_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {0};
  vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_create_info.pVertexBindingDescriptions = vector_buffer(&pipeline.vertex_input_binding_descriptions);
  vertex_input_create_info.vertexBindingDescriptionCount = (uint32_t)vector_count(&pipeline.vertex_input_binding_descriptions);
  vertex_input_create_info.pVertexAttributeDescriptions = vector_buffer(&pipeline.vertex_input_attribute_descriptions);
  vertex_input_create_info.vertexAttributeDescriptionCount = (uint32_t)vector_count(&pipeline.vertex_input_attribute_descriptions);

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {0};
  input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_create_info.primitiveRestartEnable = 0;

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_context_surface_width;
  viewport.height = (float)g_context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_context_surface_width;
  scissor.extent.height = g_context_surface_height;

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {0};
  viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_create_info.viewportCount = 1;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.scissorCount = 1;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {0};
  rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_create_info.depthClampEnable = 0;
  rasterizer_create_info.rasterizerDiscardEnable = 0;
  rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_create_info.lineWidth = 1.0F;
  rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_create_info.depthBiasEnable = 0;
  rasterizer_create_info.depthBiasConstantFactor = 0.0F;
  rasterizer_create_info.depthBiasClamp = 0.0F;
  rasterizer_create_info.depthBiasSlopeFactor = 0.0F;

  VkPipelineMultisampleStateCreateInfo multisampling_create_info = {0};
  multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_create_info.sampleShadingEnable = 0;
  multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_create_info.minSampleShading = 1.0F;
  multisampling_create_info.pSampleMask = 0;
  multisampling_create_info.alphaToCoverageEnable = 0;
  multisampling_create_info.alphaToOneEnable = 0;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = 1;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {0};
  depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state_create_info.depthTestEnable = 1;
  depth_stencil_state_create_info.depthWriteEnable = 1;
  depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state_create_info.depthBoundsTestEnable = 0;
  depth_stencil_state_create_info.stencilTestEnable = 0;

  VkPipelineColorBlendStateCreateInfo color_blend_create_info = {0};
  color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_create_info.logicOpEnable = 0;
  color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_create_info.attachmentCount = 1;
  color_blend_create_info.pAttachments = &color_blend_attachment;
  color_blend_create_info.blendConstants[0] = 0.0F;
  color_blend_create_info.blendConstants[1] = 0.0F;
  color_blend_create_info.blendConstants[2] = 0.0F;
  color_blend_create_info.blendConstants[3] = 0.0F;

  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {0};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.pDynamicStates = dynamic_states;
  dynamic_state_create_info.dynamicStateCount = ARRAY_COUNT(dynamic_states);

  VkGraphicsPipelineCreateInfo graphic_pipeline_create_info = {0};
  graphic_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphic_pipeline_create_info.pStages = shader_stages;
  graphic_pipeline_create_info.stageCount = ARRAY_COUNT(shader_stages);
  graphic_pipeline_create_info.pVertexInputState = &vertex_input_create_info;
  graphic_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
  graphic_pipeline_create_info.pViewportState = &viewport_state_create_info;
  graphic_pipeline_create_info.pRasterizationState = &rasterizer_create_info;
  graphic_pipeline_create_info.pMultisampleState = &multisampling_create_info;
  graphic_pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
  graphic_pipeline_create_info.pColorBlendState = &color_blend_create_info;
  graphic_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  graphic_pipeline_create_info.layout = pipeline.pipeline_layout;
  graphic_pipeline_create_info.renderPass = g_swapchain_render_pass;
  graphic_pipeline_create_info.subpass = 0;
  graphic_pipeline_create_info.basePipelineHandle = 0;

  VULKAN_CHECK(vkCreateGraphicsPipelines(g_context_device, 0, 1, &graphic_pipeline_create_info, 0, &pipeline.pipeline));

  vkDestroyShaderModule(g_context_device, vertex_shader_module, 0);
  vkDestroyShaderModule(g_context_device, fragment_shader_module, 0);

  spvReflectDestroyShaderModule(&reflect_vertex_shader_module);
  spvReflectDestroyShaderModule(&reflect_fragment_shader_module);

  database_destroy_graphic_pipeline_settings(&db_settings);

  return pipeline;
}
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count) {
  uint64_t descriptor_set_layout_count = vector_count(&pipeline->descriptor_set_layout_bindings);

  vector_resize(&pipeline->descriptor_set_layouts, descriptor_count);
  vector_resize(&pipeline->descriptor_sets, descriptor_count);
  vector_resize(&pipeline->write_descriptor_sets, descriptor_count * descriptor_set_layout_count);

  vector_fill(&pipeline->descriptor_set_layouts, &pipeline->descriptor_set_layout);

  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {0};
  descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate_info.descriptorPool = pipeline->descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = (uint32_t)vector_count(&pipeline->descriptor_set_layouts);
  descriptor_set_allocate_info.pSetLayouts = vector_buffer(&pipeline->descriptor_set_layouts);

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &descriptor_set_allocate_info, vector_buffer(&pipeline->descriptor_sets)));
}
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline) {
  int32_t frame_index = 0;
  while (frame_index < pipeline->frames_in_flight) {

    vector_t *buffer_mappings = (vector_t *)map_at(&pipeline->buffer_mappings, &frame_index, sizeof(uint32_t));

    uint64_t descriptor_set_index = 0;
    uint64_t descriptor_set_count = vector_count(&pipeline->descriptor_sets);

    while (descriptor_set_index < descriptor_set_count) {

      VkDescriptorSet descriptor_set = *(VkDescriptorSet *)vector_at(&pipeline->descriptor_sets, frame_index * descriptor_set_index);

      uint64_t descriptor_set_layout_binding_index = 0;
      uint64_t descriptor_set_layout_binding_count = vector_count(&pipeline->write_descriptor_sets);

      while (descriptor_set_layout_binding_index < descriptor_set_layout_binding_count) {

        VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(&pipeline->descriptor_set_layout_bindings, descriptor_set_layout_binding_index);
        VkWriteDescriptorSet *write_descriptor_set = (VkWriteDescriptorSet *)vector_at(&pipeline->write_descriptor_sets, (frame_index * descriptor_set_index) + descriptor_set_layout_binding_index);

        buffer_t *buffer = *(buffer_t **)vector_at(buffer_mappings, descriptor_set_layout_binding->binding);

        switch (descriptor_set_layout_binding->descriptorType) {
          case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            VkDescriptorBufferInfo descriptor_buffer_info = {0};

            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.buffer = buffer_handle(buffer);
            descriptor_buffer_info.range = VK_WHOLE_SIZE;

            memset(write_descriptor_set, 0, sizeof(VkWriteDescriptorSet));

            write_descriptor_set->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_set->pNext = 0;
            write_descriptor_set->dstSet = descriptor_set;
            write_descriptor_set->dstBinding = descriptor_set_layout_binding->binding;
            write_descriptor_set->dstArrayElement = 0;
            write_descriptor_set->descriptorCount = 1; // TODO: support arrays..
            write_descriptor_set->descriptorType = descriptor_set_layout_binding->descriptorType;
            write_descriptor_set->pImageInfo = 0;
            write_descriptor_set->pBufferInfo = &descriptor_buffer_info;
            write_descriptor_set->pTexelBufferView = 0;

            break;
          }
        }

        descriptor_set_layout_binding_index++;
      }

      descriptor_set_index++;
    }

    frame_index++;
  }

  vkUpdateDescriptorSets(g_context_device, (int32_t)vector_count(&pipeline->write_descriptor_sets), vector_buffer(&pipeline->write_descriptor_sets), 0, 0);
}
void *graphic_pipeline_vertex_buffer(graphic_pipeline_t *pipeline, int32_t frame_index, uint64_t binding_index) {
  // TODO
  return 0;
}
void *graphic_pipeline_index_buffer(graphic_pipeline_t *pipeline, int32_t frame_index) {
  // TODO
  return 0;
}
uint64_t *graphic_pipeline_vertex_offset(graphic_pipeline_t *pipeline, int32_t frame_index) {
  return 0;
}
uint64_t *graphic_pipeline_index_offset(graphic_pipeline_t *pipeline, int32_t frame_index) {
  return 0;
}
void graphic_pipeline_execute(graphic_pipeline_t *pipeline) {
  // TODO
  // VkBuffer vertex_buffers[] = {buffer_handle(&frame->debug_line_vertex_buffer)};
  // uint64_t vertex_offsets[] = {0};
  //
  // VkBuffer index_buffer = buffer_handle(&frame->debug_line_index_buffer);
  //
  // char const *debug_line_pipeline_name = "debug_line";
  //
  // uint64_t debug_line_pipeline_name_length = strlen(debug_line_pipeline_name);
  //
  // pipeline_t *debug_line_pipeline = (pipeline_t *)map_at(&s_renderer_pipelines, debug_line_pipeline_name, debug_line_pipeline_name_length);
  //
  // pipeline_execute(debug_line_pipeline, s_renderer_graphic_command_buffers[s_renderer_frame_index], vertex_buffers, ARRAY_COUNT(vertex_buffers), vertex_offsets, index_buffer, frame->debug_line_index_offset);
  //
  // frame->debug_line_vertex_offset = 0;
  // frame->debug_line_index_offset = 0;
}
void graphic_pipeline_destroy(graphic_pipeline_t *pipeline) {
  vector_destroy(&pipeline->vertex_input_binding_descriptions);
  vector_destroy(&pipeline->vertex_input_attribute_descriptions);
  vector_destroy(&pipeline->descriptor_pool_sizes);
  vector_destroy(&pipeline->descriptor_set_layout_bindings);
  vector_destroy(&pipeline->descriptor_set_layouts);
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  map_iter_t iter = map_iter_create_from(&pipeline->buffer_mappings);

  if (map_iter_valid(&iter)) {
    vector_t *buffer_mappings = (vector_t *)map_iter_value(&iter);

    vector_destroy(buffer_mappings);

    map_iter_advance(&iter);
  }

  map_destroy(&pipeline->descriptor_name_to_binding);
  map_destroy(&pipeline->buffer_mappings);

  vkDestroyDescriptorPool(g_context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_context_device, pipeline->pipeline, 0);
}

compute_pipeline_t compute_pipeline_create(int32_t frames_in_flight, string_t *pipeline_name) {
  compute_pipeline_t pipeline = {0};

  database_compute_pipeline_settings_t db_settings = database_load_compute_pipeline_settings_by_name(string_buffer(pipeline_name));

  pipeline.frames_in_flight = frames_in_flight;
  pipeline.descriptor_pool_sizes = vector_create(sizeof(VkDescriptorPoolSize));
  pipeline.descriptor_set_layout_bindings = vector_create(sizeof(VkDescriptorSetLayoutBinding));
  pipeline.descriptor_set_layouts = vector_create(sizeof(VkDescriptorSetLayout));
  pipeline.descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline.write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));
  pipeline.descriptor_name_to_binding = map_create();
  pipeline.buffer_mappings = map_create();

  map_t descriptor_pool_size_mappings = map_create();
  map_t descriptor_set_layout_binding_mappings = map_create();

  SpvReflectShaderModule reflect_compute_shader_module = {0};

  VkShaderModule compute_shader_module = 0;

  SPIRV_CHECK(spvReflectCreateShaderModule(vector_size(&db_settings.compute_shader), vector_buffer(&db_settings.compute_shader), &reflect_compute_shader_module));

  pipeline_insert_descriptor_pool_size(&descriptor_pool_size_mappings, &reflect_compute_shader_module);
  pipeline_insert_descriptor_set_layout_binding(&descriptor_set_layout_binding_mappings, &reflect_compute_shader_module, VK_SHADER_STAGE_COMPUTE_BIT);

  pipeline_flatten_descriptor_pool_sizes(&descriptor_pool_size_mappings, &pipeline.descriptor_pool_sizes);
  pipeline_flatten_descriptor_set_layout_bindings(&descriptor_set_layout_binding_mappings, &pipeline.descriptor_set_layout_bindings);

  pipeline_insert_descriptor_binding_names(&pipeline.descriptor_name_to_binding, &reflect_compute_shader_module);

#ifdef BUILD_DEBUG
  printf("%s\n", string_buffer(&db_settings.pipeline_name));
  pipeline_print_descriptor_pool_sizes(&pipeline.descriptor_pool_sizes);
  pipeline_print_descriptor_set_layout_bindings(&pipeline.descriptor_set_layout_bindings);
  pipeline_print_descriptor_binding_names(&pipeline.descriptor_name_to_binding);
  printf("\n");
#endif // BUILD_DEBUG

  map_destroy(&descriptor_pool_size_mappings);
  map_destroy(&descriptor_set_layout_binding_mappings);

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&pipeline.descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&pipeline.descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &descriptor_pool_create_info, 0, &pipeline.descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&pipeline.descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&pipeline.descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &descriptor_set_layout_create_info, 0, &pipeline.descriptor_set_layout));

  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &pipeline.descriptor_set_layout;
  pipeline_layout_create_info.pPushConstantRanges = 0;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &pipeline_layout_create_info, 0, &pipeline.pipeline_layout));

  VkShaderModuleCreateInfo compute_shader_module_create_info = {0};
  compute_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  compute_shader_module_create_info.codeSize = vector_size(&db_settings.compute_shader);
  compute_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&db_settings.compute_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_context_device, &compute_shader_module_create_info, 0, &compute_shader_module));

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {0};
  compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_shader_stage_create_info.module = compute_shader_module;
  compute_shader_stage_create_info.pName = "main";

  VkComputePipelineCreateInfo compute_pipeline_create_info = {0};
  compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_create_info.layout = pipeline.pipeline_layout;
  compute_pipeline_create_info.stage = compute_shader_stage_create_info;

  VULKAN_CHECK(vkCreateComputePipelines(g_context_device, 0, 1, &compute_pipeline_create_info, 0, &pipeline.pipeline));

  vkDestroyShaderModule(g_context_device, compute_shader_module, 0);

  spvReflectDestroyShaderModule(&reflect_compute_shader_module);

  database_destroy_compute_pipeline_settings(&db_settings);

  return pipeline;
}
void compute_pipeline_allocate_descriptor_sets(compute_pipeline_t *pipeline, uint64_t descriptor_count) {
  uint64_t descriptor_set_layout_count = vector_count(&pipeline->descriptor_set_layout_bindings);

  vector_resize(&pipeline->descriptor_set_layouts, descriptor_count);
  vector_resize(&pipeline->descriptor_sets, descriptor_count);
  vector_resize(&pipeline->write_descriptor_sets, descriptor_count * descriptor_set_layout_count);

  vector_fill(&pipeline->descriptor_set_layouts, &pipeline->descriptor_set_layout);

  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {0};
  descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate_info.descriptorPool = pipeline->descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = (uint32_t)vector_count(&pipeline->descriptor_set_layouts);
  descriptor_set_allocate_info.pSetLayouts = vector_buffer(&pipeline->descriptor_set_layouts);

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &descriptor_set_allocate_info, vector_buffer(&pipeline->descriptor_sets)));
}
void compute_pipeline_update_descriptor_sets(compute_pipeline_t *pipeline) {
  int32_t frame_index = 0;
  while (frame_index < pipeline->frames_in_flight) {

    vector_t *buffer_mappings = (vector_t *)map_at(&pipeline->buffer_mappings, &frame_index, sizeof(uint32_t));

    uint64_t descriptor_set_index = 0;
    uint64_t descriptor_set_count = vector_count(&pipeline->descriptor_sets);

    while (descriptor_set_index < descriptor_set_count) {

      VkDescriptorSet descriptor_set = *(VkDescriptorSet *)vector_at(&pipeline->descriptor_sets, frame_index * descriptor_set_index);

      uint64_t descriptor_set_layout_binding_index = 0;
      uint64_t descriptor_set_layout_binding_count = vector_count(&pipeline->write_descriptor_sets);

      while (descriptor_set_layout_binding_index < descriptor_set_layout_binding_count) {

        VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(&pipeline->descriptor_set_layout_bindings, descriptor_set_layout_binding_index);
        VkWriteDescriptorSet *write_descriptor_set = (VkWriteDescriptorSet *)vector_at(&pipeline->write_descriptor_sets, (frame_index * descriptor_set_index) + descriptor_set_layout_binding_index);

        buffer_t *buffer = *(buffer_t **)vector_at(buffer_mappings, descriptor_set_layout_binding->binding);

        switch (descriptor_set_layout_binding->descriptorType) {
          case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            VkDescriptorBufferInfo descriptor_buffer_info = {0};

            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.buffer = buffer_handle(buffer);
            descriptor_buffer_info.range = VK_WHOLE_SIZE;

            memset(write_descriptor_set, 0, sizeof(VkWriteDescriptorSet));

            write_descriptor_set->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_set->pNext = 0;
            write_descriptor_set->dstSet = descriptor_set;
            write_descriptor_set->dstBinding = descriptor_set_layout_binding->binding;
            write_descriptor_set->dstArrayElement = 0;
            write_descriptor_set->descriptorCount = 1; // TODO: support arrays..
            write_descriptor_set->descriptorType = descriptor_set_layout_binding->descriptorType;
            write_descriptor_set->pImageInfo = 0;
            write_descriptor_set->pBufferInfo = &descriptor_buffer_info;
            write_descriptor_set->pTexelBufferView = 0;

            break;
          }
        }

        descriptor_set_layout_binding_index++;
      }

      descriptor_set_index++;
    }

    frame_index++;
  }

  vkUpdateDescriptorSets(g_context_device, (int32_t)vector_count(&pipeline->write_descriptor_sets), vector_buffer(&pipeline->write_descriptor_sets), 0, 0);
}
void compute_pipeline_execute(compute_pipeline_t *pipeline, VkCommandBuffer command_buffer, VkBuffer *vertex_buffers, uint64_t vertex_buffer_count, uint64_t *vertex_offsets, VkBuffer index_buffer, uint64_t index_buffer_offset) {
  // TODO
  // vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
  // vkCmdBindVertexBuffers(command_buffer, 0, (uint32_t)vertex_buffer_count, vertex_buffers, vertex_offsets);
  // vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
  // vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline_layout, 0, (uint32_t)vector_count(&pipeline->descriptor_sets), vector_buffer(&pipeline->descriptor_sets), 0, 0);
  //
  // vkCmdDrawIndexed(command_buffer, (uint32_t)index_buffer_offset, 1, 0, 0, 0);
}
void compute_pipeline_destroy(compute_pipeline_t *pipeline) {
  vector_destroy(&pipeline->descriptor_pool_sizes);
  vector_destroy(&pipeline->descriptor_set_layout_bindings);
  vector_destroy(&pipeline->descriptor_set_layouts);
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  map_iter_t iter = map_iter_create_from(&pipeline->buffer_mappings);

  if (map_iter_valid(&iter)) {
    vector_t *buffer_mappings = (vector_t *)map_iter_value(&iter);

    vector_destroy(buffer_mappings);

    map_iter_advance(&iter);
  }

  map_destroy(&pipeline->descriptor_name_to_binding);
  map_destroy(&pipeline->buffer_mappings);

  vkDestroyDescriptorPool(g_context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_context_device, pipeline->pipeline, 0);
}

// TODO
// void pipeline_link_buffer(pipeline_t *pipeline, int32_t frame_index, char const *binding_name, buffer_t *buffer) {
//   if (map_contains(&pipeline->buffer_mappings, &frame_index, sizeof(int32_t)) == 0) {
//     vector_t buffer_mappings = vector_create(sizeof(buffer_t *));
//
//     uint64_t descriptor_set_layout_binding_count = vector_count(&pipeline->descriptor_set_layout_bindings);
//
//     vector_resize(&buffer_mappings, descriptor_set_layout_binding_count);
//
//     map_insert(&pipeline->buffer_mappings, &frame_index, sizeof(int32_t), &buffer_mappings, sizeof(vector_t));
//   }
//
//   vector_t *buffer_mappings = (vector_t *)map_at(&pipeline->buffer_mappings, &frame_index, sizeof(int32_t));
//
//   uint32_t binding = pipeline_descriptor_name_to_binding(&pipeline->descriptor_name_to_binding, binding_name);
//
//   vector_set(buffer_mappings, binding, &buffer);
// }

static uint32_t pipeline_descriptor_name_to_binding(map_t *descriptor_name_to_binding, char const *binding_name) {
  uint64_t binding_name_length = strlen(binding_name) + 1;

  if (map_contains(descriptor_name_to_binding, binding_name, binding_name_length)) {
    return *(uint32_t *)map_at(descriptor_name_to_binding, binding_name, binding_name_length);
  } else {
    return 0xFFFFFFFF;
  }
}

static void pipeline_insert_descriptor_binding_names(map_t *descriptor_name_to_binding, SpvReflectShaderModule *shader_module) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    char const *name = descriptor_binding->name;
    uint64_t name_length = strlen(descriptor_binding->name) + 1;

    uint32_t binding = descriptor_binding->binding;

    if (map_contains(descriptor_name_to_binding, name, name_length) == 0) {
      map_insert(descriptor_name_to_binding, name, name_length, &binding, sizeof(uint32_t));
    }

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}

static void pipeline_print_descriptor_binding_names(map_t *descriptor_name_to_binding) {
  printf("  DescriptorBindingNames\n");

  map_iter_t iter = map_iter_create_from(descriptor_name_to_binding);

  while (map_iter_valid(&iter)) {
    char const *name = (char const *)map_iter_key(&iter);
    uint32_t binding = *(uint32_t *)map_iter_value(&iter);

    printf("    {%u, %s}\n",
           binding,
           name);

    map_iter_advance(&iter);
  }
}

static void pipeline_push_vertex_input_binding_description(vector_t *vertex_input_binding_descriptions, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint32_t current_byte_offset = 0;
  uint32_t input_variable_index = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    current_byte_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    input_variable_index++;
  }

  input_variable_index = 0;

  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    VkVertexInputBindingDescription vertex_input_binding_description = {0};

    vertex_input_binding_description.binding = 0; // TODO: WHAT?
    vertex_input_binding_description.stride = current_byte_offset;
    vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO: support instance rate..

    vector_push(vertex_input_binding_descriptions, &vertex_input_binding_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}
static void pipeline_push_vertex_input_attribute_description(vector_t *vertex_input_attribute_descriptions, SpvReflectShaderModule *shader_module) {
  uint32_t input_variable_count = 0;
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, 0));

  SpvReflectInterfaceVariable **input_variables = (SpvReflectInterfaceVariable **)heap_alloc(sizeof(SpvReflectInterfaceVariable *) * input_variable_count);
  SPIRV_CHECK(spvReflectEnumerateInputVariables(shader_module, &input_variable_count, input_variables));

  uint32_t current_byte_offset = 0;

  uint32_t input_variable_index = 0;
  while (input_variable_index < input_variable_count) {
    SpvReflectInterfaceVariable *input_variable = input_variables[input_variable_index];

    VkVertexInputAttributeDescription vertex_input_attribute_description = {0};

    vertex_input_attribute_description.location = input_variable->location;
    vertex_input_attribute_description.binding = 0;
    vertex_input_attribute_description.format = (VkFormat)input_variable->format;
    vertex_input_attribute_description.offset = current_byte_offset;

    current_byte_offset += (input_variable->numeric.scalar.width / 8) * input_variable->numeric.vector.component_count;

    vector_push(vertex_input_attribute_descriptions, &vertex_input_attribute_description);

    input_variable_index++;
  }

  heap_free(input_variables);
}

static void pipeline_print_vertex_input_binding_descriptions(vector_t *vertex_input_binding_descriptions) {
  printf("  VertexInputBindingDescriptions\n");

  uint64_t vertex_input_binding_description_index = 0;
  uint64_t vertex_input_binding_description_count = vector_count(vertex_input_binding_descriptions);

  while (vertex_input_binding_description_index < vertex_input_binding_description_count) {
    VkVertexInputBindingDescription *vertex_input_binding_description = (VkVertexInputBindingDescription *)vector_at(vertex_input_binding_descriptions, vertex_input_binding_description_index);

    printf("    {%u, %u, %u},\n",
           vertex_input_binding_description->binding,
           vertex_input_binding_description->stride,
           (uint32_t)vertex_input_binding_description->inputRate);

    vertex_input_binding_description_index++;
  }
}
static void pipeline_print_vertex_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions) {
  printf("  VertexInputAttributeDescriptions\n");

  uint64_t vertex_input_attribute_description_index = 0;
  uint64_t vertex_input_attribute_description_count = vector_count(vertex_input_attribute_descriptions);

  while (vertex_input_attribute_description_index < vertex_input_attribute_description_count) {
    VkVertexInputAttributeDescription *vertex_input_attribute_description = (VkVertexInputAttributeDescription *)vector_at(vertex_input_attribute_descriptions, vertex_input_attribute_description_index);

    printf("    {%u, %u, %u, %u}\n",
           vertex_input_attribute_description->location,
           vertex_input_attribute_description->binding,
           vertex_input_attribute_description->format,
           vertex_input_attribute_description->offset);

    vertex_input_attribute_description_index++;
  }
}

static void pipeline_insert_descriptor_pool_size(map_t *descriptor_pool_sizes, SpvReflectShaderModule *shader_module) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    VkDescriptorType type = (VkDescriptorType)descriptor_binding->descriptor_type;

    if (map_contains(descriptor_pool_sizes, &type, sizeof(VkDescriptorType))) {
      VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)map_at(descriptor_pool_sizes, &type, sizeof(VkDescriptorType));

      descriptor_pool_size->descriptorCount++;
    } else {
      VkDescriptorPoolSize descriptor_pool_size = {0};

      descriptor_pool_size.type = type;
      descriptor_pool_size.descriptorCount = 1;

      map_insert(descriptor_pool_sizes, &type, sizeof(VkDescriptorType), &descriptor_pool_size, sizeof(VkDescriptorPoolSize));
    }

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}
static void pipeline_insert_descriptor_set_layout_binding(map_t *descriptor_set_layout_bindings, SpvReflectShaderModule *shader_module, VkShaderStageFlags stage_flag) {
  uint32_t descriptor_binding_count = 0;
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, 0));

  SpvReflectDescriptorBinding **descriptor_bindings = (SpvReflectDescriptorBinding **)heap_alloc(sizeof(SpvReflectDescriptorBinding *) * descriptor_binding_count);
  SPIRV_CHECK(spvReflectEnumerateDescriptorBindings(shader_module, &descriptor_binding_count, descriptor_bindings));

  uint32_t descriptor_binding_index = 0;
  while (descriptor_binding_index < descriptor_binding_count) {
    SpvReflectDescriptorBinding *descriptor_binding = descriptor_bindings[descriptor_binding_index];

    uint32_t binding = descriptor_binding->binding;
    VkDescriptorType type = (VkDescriptorType)descriptor_binding->descriptor_type;

    if (map_contains(descriptor_set_layout_bindings, &binding, sizeof(uint32_t))) {
      VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)map_at(descriptor_set_layout_bindings, &binding, sizeof(uint32_t));

      descriptor_set_layout_binding->stageFlags |= stage_flag;
    } else {
      VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {0};

      descriptor_set_layout_binding.binding = binding;
      descriptor_set_layout_binding.descriptorType = type;
      descriptor_set_layout_binding.descriptorCount = 1; // TODO: support arrays..
      descriptor_set_layout_binding.stageFlags |= stage_flag;
      descriptor_set_layout_binding.pImmutableSamplers = 0;

      map_insert(descriptor_set_layout_bindings, &binding, sizeof(uint32_t), &descriptor_set_layout_binding, sizeof(VkDescriptorSetLayoutBinding));
    }

    descriptor_binding_index++;
  }

  heap_free(descriptor_bindings);
}

static void pipeline_print_descriptor_pool_sizes(vector_t *descriptor_pool_sizes) {
  printf("  DescriptorPoolSizes\n");

  uint64_t descriptor_pool_size_index = 0;
  uint64_t descriptor_pool_size_count = vector_count(descriptor_pool_sizes);

  while (descriptor_pool_size_index < descriptor_pool_size_count) {
    VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)vector_at(descriptor_pool_sizes, descriptor_pool_size_index);

    printf("    {%u, %u}\n",
           (uint32_t)descriptor_pool_size->type,
           descriptor_pool_size->descriptorCount);

    descriptor_pool_size_index++;
  }
}
static void pipeline_print_descriptor_set_layout_bindings(vector_t *descriptor_set_layout_bindings) {
  printf("  DescriptorSetLayoutBindings\n");

  uint64_t descriptor_set_layout_binding_index = 0;
  uint64_t descriptor_set_layout_binding_count = vector_count(descriptor_set_layout_bindings);

  while (descriptor_set_layout_binding_index < descriptor_set_layout_binding_count) {
    VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(descriptor_set_layout_bindings, descriptor_set_layout_binding_index);

    printf("    {%u, %u, %u, %u, %p}\n",
           descriptor_set_layout_binding->binding,
           (uint32_t)descriptor_set_layout_binding->descriptorType,
           descriptor_set_layout_binding->descriptorCount,
           descriptor_set_layout_binding->stageFlags,
           descriptor_set_layout_binding->pImmutableSamplers);

    descriptor_set_layout_binding_index++;
  }
}

static void pipeline_flatten_descriptor_pool_sizes(map_t *descriptor_pool_size_mappings, vector_t *descriptor_pool_sizes) {
  map_iter_t iter = map_iter_create_from(descriptor_pool_size_mappings);

  while (map_iter_valid(&iter)) {
    VkDescriptorPoolSize *descriptor_pool_size = (VkDescriptorPoolSize *)map_iter_value(&iter);

    vector_push(descriptor_pool_sizes, descriptor_pool_size);

    map_iter_advance(&iter);
  }
}
static void pipeline_flatten_descriptor_set_layout_bindings(map_t *descriptor_set_layout_bindings_mappings, vector_t *descriptor_set_layout_bindings) {
  map_iter_t iter = map_iter_create_from(descriptor_set_layout_bindings_mappings);

  while (map_iter_valid(&iter)) {
    VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)map_iter_value(&iter);

    vector_push(descriptor_set_layout_bindings, descriptor_set_layout_binding);

    map_iter_advance(&iter);
  }
}
