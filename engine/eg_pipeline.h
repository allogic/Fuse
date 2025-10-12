#ifndef EG_PIPELINE_H
#define EG_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

graphic_pipeline_t *graphic_pipeline_create(pipeline_asset_t *pipeline_asset);
void graphic_pipeline_set_auto_link_descriptor_bindings(graphic_pipeline_t *pipeline, map_t *auto_link_descriptor_binding_buffers_per_frame);
void graphic_pipeline_link_vertex_input_binding_buffer(graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer, uint64_t offset);
void graphic_pipeline_link_index_buffer(graphic_pipeline_t *pipeline, uint64_t index, VkBuffer buffer);
void graphic_pipeline_link_descriptor_binding_buffer(graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer);
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count);
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline);
void graphic_pipeline_execute(graphic_pipeline_t *pipeline, VkCommandBuffer command_buffer, uint32_t index_count);
void graphic_pipeline_destroy(graphic_pipeline_t *pipeline);

compute_pipeline_t *compute_pipeline_create(pipeline_asset_t *pipeline_asset);
void compute_pipeline_set_auto_link_descriptor_bindings(compute_pipeline_t *pipeline, map_t *auto_link_descriptor_binding_buffers_per_frame);
void compute_pipeline_allocate_descriptor_sets(compute_pipeline_t *pipeline, uint64_t descriptor_count);
void compute_pipeline_update_descriptor_sets(compute_pipeline_t *pipeline);
void compute_pipeline_execute(compute_pipeline_t *pipeline, VkCommandBuffer command_buffer);
void compute_pipeline_destroy(compute_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PIPELINE_H
