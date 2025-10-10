#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_swapchain.h>

// TODO: eliminate some vector_count ops with global uint64_t's

static vector_t pipeline_create_vertex_input_binding_descriptions(pipeline_asset_t *pipeline_asset);
static vector_t pipeline_create_vertex_input_attribute_descriptions(pipeline_asset_t *pipeline_asset);
static vector_t pipeline_create_descriptor_pool_sizes(pipeline_asset_t *pipeline_asset);
static vector_t pipeline_create_descriptor_set_layout_bindings(pipeline_asset_t *pipeline_asset);

static void graphic_pipeline_create_frame_dependant_buffers(graphic_pipeline_t *pipeline, pipeline_asset_t *pipeline_asset);
static void graphic_pipeline_build(graphic_pipeline_t *pipeline, pipeline_resource_t *pipeline_resource);

static void graphic_pipeline_destroy_frame_dependant_buffers(graphic_pipeline_t *pipeline);

static void compute_pipeline_build(compute_pipeline_t *pipeline, pipeline_resource_t *pipeline_resource);

graphic_pipeline_t *graphic_pipeline_create(pipeline_asset_t *pipeline_asset) {
  graphic_pipeline_t *pipeline = (graphic_pipeline_t *)heap_alloc(sizeof(graphic_pipeline_t), 1, 0);

  pipeline_resource_t pipeline_resource = database_load_pipeline_resource_by_id(pipeline_asset->id);

  pipeline->vertex_input_binding_count = database_load_vertex_input_binding_count_by_id(pipeline_asset->id);
  pipeline->descriptor_binding_count = database_load_descriptor_binding_count_by_id(pipeline_asset->id);
  pipeline->vertex_input_binding_buffers = vector_create(sizeof(vector_t));
  pipeline->vertex_input_binding_offsets = vector_create(sizeof(vector_t));
  pipeline->index_buffers = vector_create(sizeof(VkBuffer));
  pipeline->descriptor_binding_buffers = vector_create(sizeof(vector_t));
  pipeline->vertex_input_binding_descriptions = pipeline_create_vertex_input_binding_descriptions(pipeline_asset);
  pipeline->vertex_input_attribute_descriptions = pipeline_create_vertex_input_attribute_descriptions(pipeline_asset);
  pipeline->descriptor_pool_sizes = pipeline_create_descriptor_pool_sizes(pipeline_asset);
  pipeline->descriptor_set_layout_bindings = pipeline_create_descriptor_set_layout_bindings(pipeline_asset);
  pipeline->descriptor_set_layouts = vector_create(sizeof(VkDescriptorSetLayout));
  pipeline->descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline->write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));

  graphic_pipeline_create_frame_dependant_buffers(pipeline, &pipeline_asset);

  graphic_pipeline_build(pipeline, &pipeline_resource);

  database_destroy_pipeline_resource(&pipeline_resource);

  return pipeline;
}
void graphic_pipeline_link_vertex_input_binding_buffer(graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer, uint64_t offset) {
  vector_t *curr_frame_vertex_input_binding_buffers = (vector_t *)vector_at(&pipeline->vertex_input_binding_buffers, frame_index);
  vector_t *curr_frame_vertex_input_binding_offsets = (vector_t *)vector_at(&pipeline->vertex_input_binding_offsets, frame_index);

  vector_set(curr_frame_vertex_input_binding_buffers, binding_index, &buffer);
  vector_set(curr_frame_vertex_input_binding_offsets, binding_index, &offset);
}
void graphic_pipeline_link_index_buffer(graphic_pipeline_t *pipeline, uint64_t frame_index, VkBuffer buffer) {
  vector_set(&pipeline->index_buffers, frame_index, &buffer);
}
void graphic_pipeline_link_descriptor_binding_buffer(graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer) {
  vector_t *curr_frame_descriptor_binding_buffers = (vector_t *)vector_at(&pipeline->descriptor_binding_buffers, frame_index);

  vector_set(curr_frame_descriptor_binding_buffers, binding_index, &buffer);
}
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count) {
  pipeline->descriptor_set_count = descriptor_count;

  // TODO: double check these counts..
  vector_resize(&pipeline->descriptor_set_layouts, g_globals.renderer_frames_in_flight);
  vector_resize(&pipeline->descriptor_sets, g_globals.renderer_frames_in_flight);
  vector_resize(&pipeline->write_descriptor_sets, g_globals.renderer_frames_in_flight * descriptor_count * pipeline->descriptor_binding_count);

  vector_fill(&pipeline->descriptor_set_layouts, &pipeline->descriptor_set_layout);

  VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {0};
  descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate_info.descriptorPool = pipeline->descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = (uint32_t)vector_count(&pipeline->descriptor_set_layouts);
  descriptor_set_allocate_info.pSetLayouts = vector_buffer(&pipeline->descriptor_set_layouts);

  VULKAN_CHECK(vkAllocateDescriptorSets(g_globals.context_device, &descriptor_set_allocate_info, vector_buffer(&pipeline->descriptor_sets)));
}
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline) {
  uint64_t frame_index = 0;
  uint64_t frame_count = g_globals.renderer_frames_in_flight;

  while (frame_index < frame_count) {

    vector_t *curr_frame_descriptor_binding_buffers = (vector_t *)vector_at(&pipeline->descriptor_binding_buffers, frame_index);
    VkDescriptorSet descriptor_set = *(VkDescriptorSet *)vector_at(&pipeline->descriptor_sets, frame_index);

    uint64_t descriptor_set_index = 0;
    uint64_t descriptor_set_count = pipeline->descriptor_set_count;

    while (descriptor_set_index < descriptor_set_count) {

      uint64_t descriptor_binding_index = 0;
      uint64_t descriptor_binding_count = pipeline->descriptor_binding_count;

      while (descriptor_binding_index < descriptor_binding_count) {

        VkBuffer descriptor_binding_buffer = *(VkBuffer *)vector_at(curr_frame_descriptor_binding_buffers, descriptor_binding_index);
        VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(&pipeline->descriptor_set_layout_bindings, descriptor_binding_index);
        VkWriteDescriptorSet *write_descriptor_set = (VkWriteDescriptorSet *)vector_at(&pipeline->write_descriptor_sets, (frame_index) + (descriptor_set_index * frame_count) + (descriptor_binding_index * frame_count * descriptor_set_count));

        memset(write_descriptor_set, 0, sizeof(VkWriteDescriptorSet));

        switch (descriptor_set_layout_binding->descriptorType) {
          case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            VkDescriptorBufferInfo descriptor_buffer_info = {0};

            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.buffer = descriptor_binding_buffer;
            descriptor_buffer_info.range = VK_WHOLE_SIZE;

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

        descriptor_binding_index++;
      }

      descriptor_set_index++;
    }

    frame_index++;
  }

  vkUpdateDescriptorSets(g_globals.context_device, (int32_t)vector_count(&pipeline->write_descriptor_sets), vector_buffer(&pipeline->write_descriptor_sets), 0, 0);
}
void graphic_pipeline_execute(graphic_pipeline_t *pipeline, VkCommandBuffer command_buffer, uint32_t index_count) {
  vector_t *curr_frame_vertex_buffers = (vector_t *)vector_at(&pipeline->vertex_input_binding_buffers, g_globals.renderer_frame_index);
  vector_t *curr_frame_vertex_offsets = (vector_t *)vector_at(&pipeline->vertex_input_binding_offsets, g_globals.renderer_frame_index);

  VkBuffer *vertex_buffers = (VkBuffer *)vector_buffer(curr_frame_vertex_buffers);
  uint64_t *vertex_offsets = (uint64_t *)vector_buffer(curr_frame_vertex_offsets);

  VkBuffer index_buffer = *(VkBuffer *)vector_at(&pipeline->index_buffers, g_globals.renderer_frame_index);

  VkDescriptorSet *descriptor_sets = (VkDescriptorSet *)vector_at(&pipeline->descriptor_sets, g_globals.renderer_frame_index);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
  vkCmdBindVertexBuffers(command_buffer, 0, (uint32_t)pipeline->vertex_input_binding_count, vertex_buffers, vertex_offsets);
  vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline_layout, 0, 1, descriptor_sets, 0, 0);

  vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
}
void graphic_pipeline_destroy(graphic_pipeline_t *pipeline) {
  graphic_pipeline_destroy_frame_dependant_buffers(pipeline);

  vector_destroy(&pipeline->vertex_input_binding_buffers);
  vector_destroy(&pipeline->vertex_input_binding_offsets);
  vector_destroy(&pipeline->index_buffers);
  vector_destroy(&pipeline->descriptor_binding_buffers);
  vector_destroy(&pipeline->vertex_input_binding_descriptions);
  vector_destroy(&pipeline->vertex_input_attribute_descriptions);
  vector_destroy(&pipeline->descriptor_pool_sizes);
  vector_destroy(&pipeline->descriptor_set_layout_bindings);
  vector_destroy(&pipeline->descriptor_set_layouts);
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  vkDestroyDescriptorPool(g_globals.context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_globals.context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_globals.context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_globals.context_device, pipeline->pipeline, 0);

  heap_free(pipeline);
}

compute_pipeline_t *compute_pipeline_create(pipeline_asset_t *pipeline_asset) {
  compute_pipeline_t *pipeline = (compute_pipeline_t *)heap_alloc(sizeof(compute_pipeline_t), 1, 0);

  pipeline_resource_t pipeline_resource = database_load_pipeline_resource_by_id(pipeline_asset->id);

  pipeline->descriptor_pool_sizes = pipeline_create_descriptor_pool_sizes(pipeline_asset);
  pipeline->descriptor_set_layout_bindings = pipeline_create_descriptor_set_layout_bindings(pipeline_asset);
  pipeline->descriptor_sets = vector_create(sizeof(VkDescriptorSet));
  pipeline->write_descriptor_sets = vector_create(sizeof(VkWriteDescriptorSet));

  compute_pipeline_build(pipeline, &pipeline_resource);

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
void compute_pipeline_execute(compute_pipeline_t *pipeline, VkCommandBuffer command_buffer) {
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
  vector_destroy(&pipeline->descriptor_sets);
  vector_destroy(&pipeline->write_descriptor_sets);

  vkDestroyDescriptorPool(g_globals.context_device, pipeline->descriptor_pool, 0);
  vkDestroyDescriptorSetLayout(g_globals.context_device, pipeline->descriptor_set_layout, 0);
  vkDestroyPipelineLayout(g_globals.context_device, pipeline->pipeline_layout, 0);
  vkDestroyPipeline(g_globals.context_device, pipeline->pipeline, 0);

  heap_free(pipeline);
}

static vector_t pipeline_create_vertex_input_binding_descriptions(pipeline_asset_t *pipeline_asset) {
  vector_t vertex_input_binding_descriptions = vector_create(sizeof(VkVertexInputBindingDescription));

  vector_t vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);

  // TODO: Support input_rate groups..

  if (pipeline_asset->interleaved_vertex_input_buffer) {
    uint64_t vertex_input_binding_index = 0;
    uint64_t vertex_input_binding_count = vector_count(&vertex_input_bindings);

    vector_resize(&vertex_input_binding_descriptions, 1);

    uint64_t stride = 0;

    while (vertex_input_binding_index < vertex_input_binding_count) {
      pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&vertex_input_bindings, vertex_input_binding_index);

      stride += vertex_input_binding->size * vertex_input_binding->component_count;

      vertex_input_binding_index++;
    }

    VkVertexInputBindingDescription *vertex_input_binding_description = (VkVertexInputBindingDescription *)vector_at(&vertex_input_binding_descriptions, 0);

    vertex_input_binding_description->binding = 0;
    vertex_input_binding_description->stride = (uint32_t)stride;
    vertex_input_binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO
  } else {
    uint64_t vertex_input_binding_index = 0;
    uint64_t vertex_input_binding_count = vector_count(&vertex_input_bindings);

    vector_resize(&vertex_input_binding_descriptions, vertex_input_binding_count);

    while (vertex_input_binding_index < vertex_input_binding_count) {
      VkVertexInputBindingDescription *vertex_input_binding_description = (VkVertexInputBindingDescription *)vector_at(&vertex_input_binding_descriptions, vertex_input_binding_index);
      pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&vertex_input_bindings, vertex_input_binding_index);

      vertex_input_binding_description->binding = (uint32_t)vertex_input_binding_index; // TODO
      vertex_input_binding_description->stride = 0;
      vertex_input_binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO

      vertex_input_binding_index++;
    }
  }

  database_destroy_pipeline_vertex_input_bindings(&vertex_input_bindings);

  return vertex_input_binding_descriptions;
}
static vector_t pipeline_create_vertex_input_attribute_descriptions(pipeline_asset_t *pipeline_asset) {
  vector_t vertex_input_attribute_descriptions = vector_create(sizeof(VkVertexInputAttributeDescription));

  vector_t vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);

  if (pipeline_asset->interleaved_vertex_input_buffer) {
    uint64_t vertex_input_binding_index = 0;
    uint64_t vertex_input_binding_count = vector_count(&vertex_input_bindings);
    uint64_t vertex_input_binding_offset = 0;

    vector_resize(&vertex_input_attribute_descriptions, vertex_input_binding_count);

    while (vertex_input_binding_index < vertex_input_binding_count) {
      VkVertexInputAttributeDescription *vertex_input_attribute_description = (VkVertexInputAttributeDescription *)vector_at(&vertex_input_attribute_descriptions, vertex_input_binding_index);
      pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&vertex_input_bindings, vertex_input_binding_index);

      vertex_input_attribute_description->location = vertex_input_binding->location;
      vertex_input_attribute_description->binding = 0; // TODO
      vertex_input_attribute_description->format = vertex_input_binding->format;
      vertex_input_attribute_description->offset = (uint32_t)vertex_input_binding_offset;

      vertex_input_binding_offset += vertex_input_binding->size * vertex_input_binding->component_count;

      vertex_input_binding_index++;
    }
  } else {
    uint64_t vertex_input_binding_index = 0;
    uint64_t vertex_input_binding_count = vector_count(&vertex_input_bindings);
    uint64_t vertex_input_binding_offset = 0;

    vector_resize(&vertex_input_attribute_descriptions, vertex_input_binding_count);

    while (vertex_input_binding_index < vertex_input_binding_count) {
      VkVertexInputAttributeDescription *vertex_input_attribute_description = (VkVertexInputAttributeDescription *)vector_at(&vertex_input_attribute_descriptions, vertex_input_binding_index);
      pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&vertex_input_bindings, vertex_input_binding_index);

      vertex_input_attribute_description->location = vertex_input_binding->location;
      vertex_input_attribute_description->binding = 0; // TODO
      vertex_input_attribute_description->format = vertex_input_binding->format;
      vertex_input_attribute_description->offset = (uint32_t)vertex_input_binding_offset;

      vertex_input_binding_offset += vertex_input_binding->size * vertex_input_binding->component_count;

      vertex_input_binding_index++;
    }
  }

  database_destroy_pipeline_vertex_input_bindings(&vertex_input_bindings);

  return vertex_input_attribute_descriptions;
}
static vector_t pipeline_create_descriptor_pool_sizes(pipeline_asset_t *pipeline_asset) {
  vector_t descriptor_pool_sizes = vector_create(sizeof(VkDescriptorPoolSize));

  uint32_t descriptor_types[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT] = {0};

  vector_t descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(&descriptor_bindings);

  while (descriptor_binding_index < descriptor_binding_count) {
    pipeline_descriptor_binding_t *descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&descriptor_bindings, descriptor_binding_index);

    descriptor_types[descriptor_binding->descriptor_type]++;

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

  database_destroy_pipeline_descriptor_bindings(&descriptor_bindings);

  return descriptor_pool_sizes;
}
static vector_t pipeline_create_descriptor_set_layout_bindings(pipeline_asset_t *pipeline_asset) {
  vector_t descriptor_set_layout_bindings = vector_create(sizeof(VkDescriptorSetLayoutBinding));

  vector_t descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);

  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(&descriptor_bindings);

  vector_resize(&descriptor_set_layout_bindings, descriptor_binding_count);

  while (descriptor_binding_index < descriptor_binding_count) {
    VkDescriptorSetLayoutBinding *descriptor_set_layout_binding = (VkDescriptorSetLayoutBinding *)vector_at(&descriptor_set_layout_bindings, descriptor_binding_index);
    pipeline_descriptor_binding_t *descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&descriptor_bindings, descriptor_binding_index);

    descriptor_set_layout_binding->binding = descriptor_binding->binding;
    descriptor_set_layout_binding->descriptorType = descriptor_binding->descriptor_type;
    descriptor_set_layout_binding->descriptorCount = descriptor_binding->descriptor_count;
    descriptor_set_layout_binding->stageFlags = descriptor_binding->stage_flags;
    descriptor_set_layout_binding->pImmutableSamplers = 0;

    descriptor_binding_index++;
  }

  database_destroy_pipeline_descriptor_bindings(&descriptor_bindings);

  return descriptor_set_layout_bindings;
}

static void graphic_pipeline_create_frame_dependant_buffers(graphic_pipeline_t *pipeline, pipeline_asset_t *pipeline_asset) {
  vector_resize(&pipeline->vertex_input_binding_buffers, g_globals.renderer_frames_in_flight);
  vector_resize(&pipeline->vertex_input_binding_offsets, g_globals.renderer_frames_in_flight);
  vector_resize(&pipeline->index_buffers, g_globals.renderer_frames_in_flight);
  vector_resize(&pipeline->descriptor_binding_buffers, g_globals.renderer_frames_in_flight);

  uint64_t frame_index = 0;
  uint64_t frame_count = g_globals.renderer_frames_in_flight;

  while (frame_index < frame_count) {

    vector_t curr_frame_vertex_input_binding_buffers = vector_create(sizeof(VkBuffer));
    vector_t curr_frame_vertex_input_binding_offsets = vector_create(sizeof(uint64_t));
    vector_t curr_frame_descriptor_binding_buffers = vector_create(sizeof(VkBuffer));

    vector_resize(&curr_frame_vertex_input_binding_buffers, pipeline->vertex_input_binding_count);
    vector_resize(&curr_frame_vertex_input_binding_offsets, pipeline->vertex_input_binding_count);
    vector_resize(&curr_frame_descriptor_binding_buffers, pipeline->descriptor_binding_count);

    vector_set(&pipeline->vertex_input_binding_buffers, frame_index, &curr_frame_vertex_input_binding_buffers);
    vector_set(&pipeline->vertex_input_binding_offsets, frame_index, &curr_frame_vertex_input_binding_offsets);
    vector_set(&pipeline->descriptor_binding_buffers, frame_index, &curr_frame_descriptor_binding_buffers);

    if (pipeline_asset->auto_vertex_input_buffer) {
      if (pipeline_asset->interleaved_vertex_input_buffer) {
        vector_set(&pipeline->vertex_input_binding_buffers, frame_index, &curr_frame_vertex_input_binding_buffers);
      } else {
      }
    }

    frame_index++;
  }
}
static void graphic_pipeline_build(graphic_pipeline_t *pipeline, pipeline_resource_t *pipeline_resource) {
  uint8_t *vertex_shader_bytes = 0;
  uint8_t *fragment_shader_bytes = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesys_load_binary(&vertex_shader_bytes, &vertex_shader_size, pipeline_resource->vertex_shader_file_path);
  filesys_load_binary(&fragment_shader_bytes, &fragment_shader_size, pipeline_resource->fragment_shader_file_path);

  VkShaderModule vertex_shader_module = 0;
  VkShaderModule fragment_shader_module = 0;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&pipeline->descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&pipeline->descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = g_globals.renderer_frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_globals.context_device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&pipeline->descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&pipeline->descriptor_set_layout_bindings);
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
  vertex_shader_module_create_info.codeSize = vertex_shader_size;
  vertex_shader_module_create_info.pCode = (uint32_t const *)vertex_shader_bytes;

  VULKAN_CHECK(vkCreateShaderModule(g_globals.context_device, &vertex_shader_module_create_info, 0, &vertex_shader_module));

  VkShaderModuleCreateInfo fragment_shader_module_create_info = {0};
  fragment_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragment_shader_module_create_info.codeSize = fragment_shader_size;
  fragment_shader_module_create_info.pCode = (uint32_t const *)fragment_shader_bytes;

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
  vertex_input_create_info.pVertexBindingDescriptions = vector_buffer(&pipeline->vertex_input_binding_descriptions);
  vertex_input_create_info.vertexBindingDescriptionCount = (uint32_t)vector_count(&pipeline->vertex_input_binding_descriptions);
  vertex_input_create_info.pVertexAttributeDescriptions = vector_buffer(&pipeline->vertex_input_attribute_descriptions);
  vertex_input_create_info.vertexAttributeDescriptionCount = (uint32_t)vector_count(&pipeline->vertex_input_attribute_descriptions);

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

  heap_free(vertex_shader_bytes);
  heap_free(fragment_shader_bytes);
}

static void graphic_pipeline_destroy_frame_dependant_buffers(graphic_pipeline_t *pipeline) {
  uint64_t frame_index = 0;
  uint64_t frame_count = g_globals.renderer_frames_in_flight;

  while (frame_index < frame_count) {

    vector_t *curr_frame_vertex_input_binding_buffers = (vector_t *)vector_at(&pipeline->vertex_input_binding_buffers, frame_index);
    vector_t *curr_frame_vertex_input_binding_offsets = (vector_t *)vector_at(&pipeline->vertex_input_binding_offsets, frame_index);
    vector_t *curr_frame_descriptor_binding_buffers = (vector_t *)vector_at(&pipeline->descriptor_binding_buffers, frame_index);

    vector_destroy(curr_frame_vertex_input_binding_buffers);
    vector_destroy(curr_frame_vertex_input_binding_offsets);
    vector_destroy(curr_frame_descriptor_binding_buffers);

    frame_index++;
  }
}

static void compute_pipeline_build(compute_pipeline_t *pipeline, pipeline_resource_t *pipeline_resource) {
  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesys_load_binary(&compute_shader_bytes, &compute_shader_size, pipeline_resource->compute_shader_file_path);

  VkShaderModule compute_shader_module = 0;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pPoolSizes = vector_buffer(&pipeline->descriptor_pool_sizes);
  descriptor_pool_create_info.poolSizeCount = (uint32_t)vector_count(&pipeline->descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = g_globals.renderer_frames_in_flight; // TODO: Double check that..

  VULKAN_CHECK(vkCreateDescriptorPool(g_globals.context_device, &descriptor_pool_create_info, 0, &pipeline->descriptor_pool));

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.pBindings = vector_buffer(&pipeline->descriptor_set_layout_bindings);
  descriptor_set_layout_create_info.bindingCount = (uint32_t)vector_count(&pipeline->descriptor_set_layout_bindings);
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
  compute_shader_module_create_info.codeSize = compute_shader_size;
  compute_shader_module_create_info.pCode = (uint32_t const *)compute_shader_bytes;

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

  heap_free(compute_shader_bytes);
}
