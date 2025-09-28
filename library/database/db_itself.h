#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void database_create(void);
void database_destroy(void);

swapchain_asset_t database_load_swapchain_default_asset(void);
renderer_asset_t database_load_renderer_default_asset(void);

vector_t database_load_pipeline_assets(void);
vector_t database_load_pipeline_assets_by_type(uint32_t pipeline_type);
pipeline_resource_t database_load_pipeline_resource_by_id(pipeline_id_t pipeline_id);
vector_t database_load_pipeline_vertex_input_bindings_by_id(pipeline_id_t pipeline_id);
vector_t database_load_pipeline_descriptor_bindings_by_id(pipeline_id_t pipeline_id);

pipeline_id_t database_store_pipeline_asset(char const *pipeline_name, uint32_t pipeline_type, uint32_t pipeline_link_index, uint32_t pipeline_auto_create);
pipeline_resource_id_t database_store_pipeline_resource(pipeline_id_t pipeline_id, char const *vertex_shader_file_path, char const *fragment_shader_file_path, char const *compute_shader_file_path, uint8_t *vertex_shader, uint8_t *fragment_shader, uint8_t *compute_shader, uint64_t vertex_shader_size, uint64_t fragment_shader_size, uint64_t compute_shader_size);
pipeline_vertex_input_binding_id_t database_store_pipeline_vertex_input_binding(pipeline_id_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t location, uint32_t stride, uint32_t format, uint32_t offset, uint32_t input_rate);
pipeline_descriptor_binding_id_t database_store_pipeline_descriptor_binding(pipeline_id_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t descriptor_type, uint32_t descriptor_count, uint32_t stage_flags, uint8_t auto_buffer);

void database_destroy_swapchain_asset(swapchain_asset_t *swapchain_asset);
void database_destroy_renderer_asset(renderer_asset_t *renderer_asset);

void database_destroy_pipeline_assets(vector_t *pipeline_assets);
void database_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource);
void database_destroy_pipeline_vertex_input_bindings(vector_t *vertex_input_bindings);
void database_destroy_pipeline_descriptor_bindings(vector_t *descriptor_bindings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DB_H
