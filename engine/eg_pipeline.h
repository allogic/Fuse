#ifndef EG_PIPELINE_H
#define EG_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

graphic_pipeline_t *graphic_pipeline_create(uint32_t frames_in_flight, pipeline_id_t pipeline_id);
void graphic_pipeline_allocate_descriptor_sets(graphic_pipeline_t *pipeline, uint64_t descriptor_count);
void graphic_pipeline_update_descriptor_sets(graphic_pipeline_t *pipeline);
void graphic_pipeline_create_vertex_buffers(graphic_pipeline_t *pipeline, uint32_t vertex_count);
void graphic_pipeline_execute(graphic_pipeline_t *pipeline);
void graphic_pipeline_destroy(graphic_pipeline_t *pipeline);

compute_pipeline_t *compute_pipeline_create(uint32_t frames_in_flight, pipeline_id_t pipeline_id);
void compute_pipeline_allocate_descriptor_sets(compute_pipeline_t *pipeline, uint64_t descriptor_count);
void compute_pipeline_update_descriptor_sets(compute_pipeline_t *pipeline);
void compute_pipeline_execute(compute_pipeline_t *pipeline, VkCommandBuffer command_buffer, VkBuffer *vertex_buffers, uint64_t vertex_buffer_count, uint64_t *vertex_offsets, VkBuffer index_buffer, uint64_t index_buffer_offset);
void compute_pipeline_destroy(compute_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PIPELINE_H
