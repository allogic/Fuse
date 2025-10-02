#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void database_create(void);
void database_destroy(void);

swapchain_asset_t database_load_swapchain_default_asset(void);
renderer_asset_t database_load_renderer_default_asset(void);

uint64_t database_load_vertex_input_binding_count(pipeline_asset_id_t pipeline_asset_id);
uint64_t database_load_descriptor_binding_count(pipeline_asset_id_t pipeline_asset_id);

vector_t database_load_pipeline_assets(void);
vector_t database_load_pipeline_assets_by_type(uint32_t pipeline_type);
pipeline_resource_t database_load_pipeline_resource_by_id(pipeline_asset_id_t pipeline_asset_id);
vector_t database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset_id_t pipeline_asset_id);
vector_t database_load_pipeline_descriptor_bindings_by_id(pipeline_asset_id_t pipeline_asset_id);

vector_t database_load_model_assets(void);

pipeline_asset_id_t database_store_pipeline_asset(char const *pipeline_name, uint32_t pipeline_type, uint32_t pipeline_link_index, uint32_t pipeline_auto_create);
pipeline_resource_id_t database_store_pipeline_resource(pipeline_asset_id_t pipeline_asset_id, char const *vertex_shader_file_path, char const *fragment_shader_file_path, char const *compute_shader_file_path, uint8_t *vertex_shader_bytes, uint8_t *fragment_shader_bytes, uint8_t *compute_shader_bytes, uint64_t vertex_shader_size, uint64_t fragment_shader_size, uint64_t compute_shader_size);
pipeline_vertex_input_binding_id_t database_store_pipeline_vertex_input_binding(pipeline_asset_id_t pipeline_asset_id, char const *binding_name, uint32_t binding, uint32_t location, uint32_t stride, uint32_t format, uint32_t offset, uint32_t input_rate);
pipeline_descriptor_binding_id_t database_store_pipeline_descriptor_binding(pipeline_asset_id_t pipeline_asset_id, char const *binding_name, uint32_t binding, uint32_t descriptor_type, uint32_t descriptor_count, uint32_t stage_flags, uint8_t auto_buffer);

model_asset_id_t database_store_model_asset(char const *model_name);
model_resource_id_t database_store_model_resource(model_asset_id_t model_asset_id, char const *model_file_path, uint8_t *model_bytes, uint64_t model_size);
model_mesh_id_t database_store_model_mesh(model_asset_id_t model_asset_id);
model_primitive_id_t database_store_model_primitive(model_mesh_id_t model_mesh_id);
model_attribute_id_t database_store_model_attribute(model_primitive_id_t model_primitive_id, char const *attribute_name, uint32_t attribute_type, uint8_t *value_bytes, uint64_t value_size);

void database_destroy_swapchain_asset(swapchain_asset_t *swapchain_asset);

void database_destroy_renderer_asset(renderer_asset_t *renderer_asset);

void database_destroy_pipeline_assets(vector_t *pipeline_assets);
void database_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource);
void database_destroy_pipeline_vertex_input_bindings(vector_t *vertex_input_bindings);
void database_destroy_pipeline_descriptor_bindings(vector_t *descriptor_bindings);

void database_destroy_model_assets(vector_t *model_assets);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DB_H
