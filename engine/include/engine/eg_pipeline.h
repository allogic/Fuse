#ifndef EG_PIPELINE_H
#define EG_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_graphic_pipeline_t *eg_graphic_pipeline_create(eg_context_t *context, lb_pipeline_asset_id_t pipeline_asset_id);
void eg_graphic_pipeline_set_auto_link_descriptor_bindings(eg_graphic_pipeline_t *pipeline, lb_map_t **auto_link_descriptor_binding_buffers_per_frame);
void eg_graphic_pipeline_link_vertex_input_binding_buffer(eg_graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer, uint64_t offset);
void eg_graphic_pipeline_link_index_buffer(eg_graphic_pipeline_t *pipeline, uint64_t index, VkBuffer buffer);
void eg_graphic_pipeline_link_descriptor_binding_buffer(eg_graphic_pipeline_t *pipeline, uint64_t frame_index, uint64_t binding_index, VkBuffer buffer);
void eg_graphic_pipeline_allocate_descriptor_sets(eg_graphic_pipeline_t *pipeline, uint64_t descriptor_count);
void eg_graphic_pipeline_update_descriptor_sets(eg_graphic_pipeline_t *pipeline);
void eg_graphic_pipeline_execute(eg_graphic_pipeline_t *pipeline, VkCommandBuffer command_buffer, uint32_t index_count);
void eg_graphic_pipeline_destroy(eg_graphic_pipeline_t *pipeline);

eg_compute_pipeline_t *eg_compute_pipeline_create(eg_context_t *context, lb_pipeline_asset_id_t pipeline_asset_id);
void eg_compute_pipeline_set_auto_link_descriptor_bindings(eg_compute_pipeline_t *pipeline, lb_map_t **auto_link_descriptor_binding_buffers_per_frame);
void eg_compute_pipeline_allocate_descriptor_sets(eg_compute_pipeline_t *pipeline, uint64_t descriptor_count);
void eg_compute_pipeline_update_descriptor_sets(eg_compute_pipeline_t *pipeline);
void eg_compute_pipeline_execute(eg_compute_pipeline_t *pipeline, VkCommandBuffer command_buffer);
void eg_compute_pipeline_destroy(eg_compute_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PIPELINE_H
