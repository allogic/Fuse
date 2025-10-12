#ifndef DB_ITSELF_H
#define DB_ITSELF_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void database_create(void);
void database_destroy(void);

swapchain_asset_t database_load_swapchain_default_asset(void);
renderer_asset_t database_load_renderer_default_asset(void);

uint64_t database_load_vertex_input_binding_count_by_id(pipeline_asset_id_t pipeline_asset_id);
uint64_t database_load_descriptor_binding_count_by_id(pipeline_asset_id_t pipeline_asset_id);

vector_t database_load_pipeline_assets(void);
vector_t database_load_pipeline_assets_by_type(uint32_t pipeline_type);
pipeline_resource_t database_load_pipeline_resource_by_id(pipeline_asset_id_t pipeline_asset_id);
vector_t database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset_id_t pipeline_asset_id);
vector_t database_load_pipeline_descriptor_bindings_by_id(pipeline_asset_id_t pipeline_asset_id);

vector_t database_load_model_assets(void);
// TODO

void database_store_pipeline_asset(pipeline_asset_t *pipeline_asset);
void database_store_pipeline_resource(pipeline_resource_t *pipeline_resource);
void database_store_pipeline_vertex_input_binding(pipeline_vertex_input_binding_t *pipeline_vertex_input_binding);
void database_store_pipeline_descriptor_binding(pipeline_descriptor_binding_t *pipeline_descriptor_binding);

void database_store_model_asset(model_asset_t *model_asset);
void database_store_model_resource(model_resource_t *model_resource);
void database_store_model_mesh(model_mesh_t *model_mesh);
void database_store_model_primitive(model_primitive_t *model_primitive);
void database_store_model_attribute(model_attribute_t *model_attribute);

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

#endif // DB_ITSELF_H
