#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_swapchain.h>

static vector_t pipeline_create_vertex_input_binding_descriptions(pipeline_id_t pipeline_id);
static vector_t pipeline_create_vertex_input_attribute_descriptions(pipeline_id_t pipeline_id);
static vector_t pipeline_create_descriptor_pool_sizes(pipeline_id_t pipeline_id);
static vector_t pipeline_create_descriptor_set_layout_bindings(pipeline_id_t pipeline_id);

graphic_pipeline_t *graphic_pipeline_create(uint32_t frames_in_flight, pipeline_id_t pipeline_id) {
  graphic_pipeline_t *pipeline = (graphic_pipeline_t *)heap_alloc(sizeof(graphic_pipeline_t));

  pipeline_resource_t pipeline_resource = database_load_pipeline_resource_by_id(pipeline_id);

  vector_t vertex_input_binding_descriptions = pipeline_create_vertex_input_binding_descriptions(pipeline_id);
  vector_t vertex_input_attribute_descriptions = pipeline_create_vertex_input_attribute_descriptions(pipeline_id);
  vector_t descriptor_pool_sizes = pipeline_create_descriptor_pool_sizes(pipeline_id);
  vector_t descriptor_set_layout_bindings = pipeline_create_descriptor_set_layout_bindings(pipeline_id);

  pipeline->frames_in_flight = frames_in_flight;
  pipeline->descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline->write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));

  VkShaderModule vertex_shader_module = 0;
  VkShaderModule fragment_shader_module = 0;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_globals.context_device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_globals.context_device, &descriptor_set_layout_create_info, 0, &pipeline->descriptor_set_layout));

  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &pipeline->descriptor_set_layout;
  pipeline_layout_create_info.pPushConstantRanges = 0;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_globals.context_device, &pipeline_layout_create_info, 0, &pipeline->pipeline_layout));

  VkShaderModuleCreateInfo vertex_shader_module_create_info = {0};
  vertex_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertex_shader_module_create_info.codeSize = vector_size(&pipeline_resource.vertex_shader);
  vertex_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&pipeline_resource.vertex_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_globals.context_device, &vertex_shader_module_create_info, 0, &vertex_shader_module));

  VkShaderModuleCreateInfo fragment_shader_module_create_info = {0};
  fragment_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragment_shader_module_create_info.codeSize = vector_size(&pipeline_resource.fragment_shader);
  fragment_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&pipeline_resource.fragment_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_globals.context_device, &fragment_shader_module_create_info, 0, &fragment_shader_module));

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
  vertex_input_create_info.pVertexBindingDescriptions = vector_buffer(&vertex_input_binding_descriptions);
  vertex_input_create_info.vertexBindingDescriptionCount = (uint32_t)vector_count(&vertex_input_binding_descriptions);
  vertex_input_create_info.pVertexAttributeDescriptions = vector_buffer(&vertex_input_attribute_descriptions);
  vertex_input_create_info.vertexAttributeDescriptionCount = (uint32_t)vector_count(&vertex_input_attribute_descriptions);

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {0};
  input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_create_info.primitiveRestartEnable = 0;

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_globals.context_surface_width;
  viewport.height = (float)g_globals.context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_globals.context_surface_width;
  scissor.extent.height = g_globals.context_surface_height;

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
  graphic_pipeline_create_info.layout = pipeline->pipeline_layout;
  graphic_pipeline_create_info.renderPass = g_globals.swapchain_render_pass;
  graphic_pipeline_create_info.subpass = 0;
  graphic_pipeline_create_info.basePipelineHandle = 0;

  VULKAN_CHECK(vkCreateGraphicsPipelines(g_globals.context_device, 0, 1, &graphic_pipeline_create_info, 0, &pipeline->pipeline));

  vkDestroyShaderModule(g_globals.context_device, vertex_shader_module, 0);
  vkDestroyShaderModule(g_globals.context_device, fragment_shader_module, 0);

  vector_destroy(&vertex_input_binding_descriptions);
  vector_destroy(&vertex_input_attribute_descriptions);
  vector_destroy(&descriptor_pool_sizes);
  vector_destroy(&descriptor_set_layout_bindings);

  database_destroy_pipeline_resource(&pipeline_resource);

  return pipeline;
}
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count) {
  /*
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
  */
}
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline) {
  /*
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
  */
}
void graphic_pipeline_create_vertex_buffers(graphic_pipeline_t *pipeline, uint32_t vertex_count) {
  // TODO
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
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  vkDestroyDescriptorPool(g_globals.context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_globals.context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_globals.context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_globals.context_device, pipeline->pipeline, 0);

  heap_free(pipeline);
}

compute_pipeline_t *compute_pipeline_create(uint32_t frames_in_flight, pipeline_id_t pipeline_id) {
  compute_pipeline_t *pipeline = (compute_pipeline_t *)heap_alloc(sizeof(compute_pipeline_t));

  memset(pipeline, 0, sizeof(compute_pipeline_t));

  pipeline_resource_t pipeline_resource = database_load_pipeline_resource_by_id(pipeline_id);

  vector_t descriptor_pool_sizes = pipeline_create_descriptor_pool_sizes(pipeline_id);
  vector_t descriptor_set_layout_bindings = pipeline_create_descriptor_set_layout_bindings(pipeline_id);

  pipeline->frames_in_flight = frames_in_flight;
  pipeline->descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline->write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));

  VkShaderModule compute_shader_module = 0;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_globals.context_device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_globals.context_device, &descriptor_set_layout_create_info, 0, &pipeline->descriptor_set_layout));

  VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &pipeline->descriptor_set_layout;
  pipeline_layout_create_info.pPushConstantRanges = 0;
  pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_globals.context_device, &pipeline_layout_create_info, 0, &pipeline->pipeline_layout));

  VkShaderModuleCreateInfo compute_shader_module_create_info = {0};
  compute_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  compute_shader_module_create_info.codeSize = vector_size(&pipeline_resource.compute_shader);
  compute_shader_module_create_info.pCode = (uint32_t const *)vector_buffer(&pipeline_resource.compute_shader);

  VULKAN_CHECK(vkCreateShaderModule(g_globals.context_device, &compute_shader_module_create_info, 0, &compute_shader_module));

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {0};
  compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_shader_stage_create_info.module = compute_shader_module;
  compute_shader_stage_create_info.pName = "main";

  VkComputePipelineCreateInfo compute_pipeline_create_info = {0};
  compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_create_info.layout = pipeline->pipeline_layout;
  compute_pipeline_create_info.stage = compute_shader_stage_create_info;

  VULKAN_CHECK(vkCreateComputePipelines(g_globals.context_device, 0, 1, &compute_pipeline_create_info, 0, &pipeline->pipeline));

  vkDestroyShaderModule(g_globals.context_device, compute_shader_module, 0);

  vector_destroy(&descriptor_pool_sizes);
  vector_destroy(&descriptor_set_layout_bindings);

  database_destroy_pipeline_resource(&pipeline_resource);

  return pipeline;
}
void compute_pipeline_allocate_descriptor_sets(compute_pipeline_t *pipeline, uint64_t descriptor_count) {
  /*
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
  */
}
void compute_pipeline_update_descriptor_sets(compute_pipeline_t *pipeline) {
  /*
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
  */
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
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  vkDestroyDescriptorPool(g_globals.context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_globals.context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_globals.context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_globals.context_device, pipeline->pipeline, 0);

  heap_free(pipeline);
}

static vector_t pipeline_create_vertex_input_binding_descriptions(pipeline_id_t pipeline_id) {
  vector_t vertex_input_binding_descriptions = vector_create(sizeof(VkVertexInputBindingDescription));

  vector_t db_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_id);

  uint64_t vertex_input_binding_index = 0;
  uint64_t vertex_input_binding_count = vector_count(&db_vertex_input_bindings);

  vector_resize(&vertex_input_binding_descriptions, vertex_input_binding_count);

  while (vertex_input_binding_index < vertex_input_binding_count) {
    VkVertexInputBindingDescription *vertex_input_binding_description = (VkVertexInputBindingDescription *)vector_at(&vertex_input_binding_descriptions, vertex_input_binding_index);
    pipeline_vertex_input_binding_t *db_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&db_vertex_input_bindings, vertex_input_binding_index);

    vertex_input_binding_description->binding = db_vertex_input_binding->binding;
    vertex_input_binding_description->stride = db_vertex_input_binding->stride;
    vertex_input_binding_description->inputRate = db_vertex_input_binding->input_rate;

    vertex_input_binding_index++;
  }

  database_destroy_pipeline_vertex_input_bindings(&db_vertex_input_bindings);

  return vertex_input_binding_descriptions;
}
static vector_t pipeline_create_vertex_input_attribute_descriptions(pipeline_id_t pipeline_id) {
  vector_t vertex_input_attribute_descriptions = vector_create(sizeof(VkVertexInputAttributeDescription));

  vector_t db_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_id);

  uint64_t vertex_input_binding_index = 0;
  uint64_t vertex_input_binding_count = vector_count(&db_vertex_input_bindings);

  vector_resize(&vertex_input_attribute_descriptions, vertex_input_binding_count);

  while (vertex_input_binding_index < vertex_input_binding_count) {
    VkVertexInputAttributeDescription *vertex_input_attribute_description = (VkVertexInputAttributeDescription *)vector_at(&vertex_input_attribute_descriptions, vertex_input_binding_index);
    pipeline_vertex_input_binding_t *db_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&db_vertex_input_bindings, vertex_input_binding_index);

    vertex_input_attribute_description->location = db_vertex_input_binding->location;
    vertex_input_attribute_description->binding = db_vertex_input_binding->binding;
    vertex_input_attribute_description->format = db_vertex_input_binding->format;
    vertex_input_attribute_description->offset = db_vertex_input_binding->offset;

    vertex_input_binding_index++;
  }

  database_destroy_pipeline_vertex_input_bindings(&db_vertex_input_bindings);

  return vertex_input_attribute_descriptions;
}
static vector_t pipeline_create_descriptor_pool_sizes(pipeline_id_t pipeline_id) {
  vector_t descriptor_pool_sizes = vector_create(sizeof(VkDescriptorPoolSize));

  uint32_t descriptor_types[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT] = {0};

  vector_t db_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_id);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(&db_descriptor_bindings);

  while (descriptor_binding_index < descriptor_binding_count) {
    pipeline_descriptor_binding_t *db_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&db_descriptor_bindings, descriptor_binding_index);

    descriptor_types[db_descriptor_binding->descriptor_type]++;

    descriptor_binding_index++;
  }

  for (uint32_t i = 0; i < VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; i++) {
    if (descriptor_types[i] > 0) {
      VkDescriptorPoolSize descriptor_pool_size = {0};

      descriptor_pool_size.type = i;
      descriptor_pool_size.descriptorCount = descriptor_types[i];

      vector_push(&descriptor_pool_sizes, &descriptor_pool_size);
    }
  }

  database_destroy_pipeline_descriptor_bindings(&db_descriptor_bindings);

  return descriptor_pool_sizes;
}
static vector_t pipeline_create_descriptor_set_layout_bindings(pipeline_id_t pipeline_id) {
  vector_t descriptor_set_layout_bindings = vector_create(sizeof(VkDescriptorSetLayoutBinding));

  vector_t db_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_id);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(&db_descriptor_bindings);

  vector_resize(&descriptor_set_layout_bindings, descriptor_binding_count);

  while (descriptor_binding_index < descriptor_binding_count) {
    VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(&descriptor_set_layout_bindings, descriptor_binding_index);
    pipeline_descriptor_binding_t *db_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&db_descriptor_bindings, descriptor_binding_index);

    descriptor_set_layout_binding->binding = db_descriptor_binding->binding;
    descriptor_set_layout_binding->descriptorType = db_descriptor_binding->descriptor_type;
    descriptor_set_layout_binding->descriptorCount = db_descriptor_binding->descriptor_count;
    descriptor_set_layout_binding->stageFlags = db_descriptor_binding->stage_flags;
    descriptor_set_layout_binding->pImmutableSamplers = 0;

    descriptor_binding_index++;
  }

  database_destroy_pipeline_descriptor_bindings(&db_descriptor_bindings);

  return descriptor_set_layout_bindings;
}

static vector_t pipeline_create_vertex_buffer_mappings(uint32_t frames_in_flight, pipeline_id_t pipeline_id) {
  vector_t vertex_buffer_mappings = vector_create(sizeof(vector_t));

  vector_resize(&vertex_buffer_mappings, frames_in_flight);

  vector_t db_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_id);

  uint32_t frame_index = 0;
  uint32_t frame_count = frames_in_flight;

  while (frame_index < frame_count) {
    vector_t *vertex_buffer_mapping = (vector_t *)vector_at(&vertex_buffer_mappings, frame_index);

    *vertex_buffer_mapping = vector_create(sizeof(buffer_t));

    uint64_t vertex_input_binding_index = 0;
    uint64_t vertex_input_binding_count = vector_count(&db_vertex_input_bindings);

    vector_resize(vertex_buffer_mapping, vertex_input_binding_count);

    while (vertex_input_binding_index < vertex_input_binding_count) {
      pipeline_vertex_input_binding_t *db_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&db_vertex_input_bindings, vertex_input_binding_index);

      buffer_t *vertex_buffer = (buffer_t *)vector_at(vertex_buffer_mapping, db_vertex_input_binding->location);

      //*vertex_buffer = buffer_create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, db_vertex_input_binding->);

      vertex_input_binding_index++;
    }

    frame_index++;
  }

  database_destroy_pipeline_vertex_input_bindings(&db_vertex_input_bindings);

  return vertex_buffer_mappings;
}
