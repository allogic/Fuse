#ifndef LB_DATABASE_H
#define LB_DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void lb_database_create(void);
void lb_database_destroy(void);

lb_scene_id_t lb_database_load_default_scene_id(void);
lb_swapchain_id_t lb_database_load_default_swapchain_id(void);
lb_renderer_id_t lb_database_load_default_renderer_id(void);

lb_vector_t lb_database_load_all_swapchains(void);
lb_swapchain_t lb_database_load_swapchain_by_id(lb_swapchain_id_t swapchain_id);

lb_vector_t lb_database_load_all_renderer(void);
lb_renderer_t lb_database_load_renderer_by_id(lb_renderer_id_t renderer_id);

uint64_t lb_database_load_vertex_input_binding_count_by_id(lb_pipeline_id_t pipeline_id);
uint64_t lb_database_load_descriptor_binding_count_by_id(lb_pipeline_id_t pipeline_id);

lb_vector_t lb_database_load_all_pipelines(void);
lb_vector_t lb_database_load_all_pipeline_by_type(lb_pipeline_type_t pipeline_type);
lb_pipeline_resource_t lb_database_load_pipeline_resource_by_id(lb_pipeline_id_t pipeline_id);
lb_vector_t lb_database_load_all_pipeline_vertex_input_bindings_by_id(lb_pipeline_id_t pipeline_id);
lb_vector_t lb_database_load_all_pipeline_descriptor_bindings_by_id(lb_pipeline_id_t pipeline_id);

lb_vector_t lb_database_load_all_models(void);
lb_model_resource_t lb_database_load_model_resource_by_id(lb_model_id_t model_id);
lb_vector_t lb_database_load_all_model_meshes_by_id(lb_model_id_t model_id);

lb_vector_t lb_database_load_all_mesh_primitives_by_id(lb_model_mesh_id_t model_mesh_id);
lb_vector_t lb_database_load_all_mesh_attributes_by_id(lb_mesh_primitive_id_t mesh_primitive_id);

lb_attribute_buffer_t lb_database_load_attribute_buffer_by_id(lb_mesh_attribute_id_t mesh_attribute_id);

lb_vector_t lb_database_load_all_scenes(void);

lb_graph_t lb_database_load_graph_by_id(lb_graph_id_t graph_id);

void lb_database_store_swapchain(lb_swapchain_t *swapchain);

void lb_database_store_renderer(lb_renderer_t *renderer);

void lb_database_store_pipeline(lb_pipeline_t *pipeline);
void lb_database_store_pipeline_resource(lb_pipeline_resource_t *pipeline_resource);
void lb_database_store_pipeline_vertex_input_binding(lb_pipeline_vertex_input_binding_t *pipeline_vertex_input_binding);
void lb_database_store_pipeline_descriptor_binding(lb_pipeline_descriptor_binding_t *pipeline_descriptor_binding);

void lb_database_store_model(lb_model_t *model);
void lb_database_store_model_resource(lb_model_resource_t *model_resource);
void lb_database_store_model_mesh(lb_model_mesh_t *model_mesh);

void lb_database_store_mesh_primitive(lb_mesh_primitive_t *mesh_primitive);
void lb_database_store_mesh_attribute(lb_mesh_attribute_t *mesh_attribute);

void lb_database_store_attribute_buffer(lb_attribute_buffer_t *attribute_buffer);

void lb_database_store_scene(lb_scene_t *scene);

void lb_database_store_graph(lb_graph_t *graph);

void lb_database_destroy_swapchain(lb_swapchain_t *swapchain);
void lb_database_destroy_swapchainv(lb_vector_t *swapchains);

void lb_database_destroy_renderer(lb_renderer_t *renderer);
void lb_database_destroy_rendererv(lb_vector_t *renderer);

void lb_database_destroy_pipelinev(lb_vector_t *pipelines);
void lb_database_destroy_pipeline_resource(lb_pipeline_resource_t *pipeline_resource);
void lb_database_destroy_pipeline_vertex_input_bindingv(lb_vector_t *vertex_input_bindings);
void lb_database_destroy_pipeline_descriptor_bindingv(lb_vector_t *descriptor_bindings);

void lb_database_destroy_modelv(lb_vector_t *models);
void lb_database_destroy_model_resource(lb_model_resource_t *model_resource);
void lb_database_destroy_model_meshv(lb_vector_t *model_meshes);

void lb_database_destroy_mesh_primitivesv(lb_vector_t *mesh_primitives);
void lb_database_destroy_mesh_attributesv(lb_vector_t *mesh_attributes);

void lb_database_destroy_attribute_buffer(lb_attribute_buffer_t *attribute_buffer);

void lb_database_destroy_scenev(lb_vector_t *scenes);

void lb_database_destroy_graphv(lb_vector_t *graphs);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_DATABASE_H
