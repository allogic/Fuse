#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <engine/buffer.h>

typedef struct graphic_pipeline_t {
  int32_t frames_in_flight;
  vector_t vertex_input_binding_descriptions;
  vector_t vertex_input_attribute_descriptions;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  vector_t vertex_buffers;
  vector_t index_buffers;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} graphic_pipeline_t;

typedef struct compute_pipeline_t {
  int32_t frames_in_flight;
  vector_t descriptor_pool_sizes;
  vector_t descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline pipeline;
} compute_pipeline_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

graphic_pipeline_t graphic_pipeline_create(int32_t frames_in_flight, int32_t pipeline_id);
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count);
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline);
void *graphic_pipeline_vertex_buffer(graphic_pipeline_t *pipeline, int32_t frame_index, uint64_t binding_index); // TODO
void *graphic_pipeline_index_buffer(graphic_pipeline_t *pipeline, int32_t frame_index);                          // TODO
uint64_t *graphic_pipeline_vertex_offset(graphic_pipeline_t *pipeline, int32_t frame_index);                     // TODO
uint64_t *graphic_pipeline_index_offset(graphic_pipeline_t *pipeline, int32_t frame_index);                      // TODO
void graphic_pipeline_execute(graphic_pipeline_t *pipeline);
void graphic_pipeline_destroy(graphic_pipeline_t *pipeline);

compute_pipeline_t compute_pipeline_create(int32_t frames_in_flight, int32_t pipeline_id);
void compute_pipeline_allocate_descriptor_sets(compute_pipeline_t *pipeline, uint64_t descriptor_count);
void compute_pipeline_update_descriptor_sets(compute_pipeline_t *pipeline);
void compute_pipeline_execute(compute_pipeline_t *pipeline, VkCommandBuffer command_buffer, VkBuffer *vertex_buffers, uint64_t vertex_buffer_count, uint64_t *vertex_offsets, VkBuffer index_buffer, uint64_t index_buffer_offset);
void compute_pipeline_destroy(compute_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PIPELINE_H
