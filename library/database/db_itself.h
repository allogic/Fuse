#ifndef DB_H
#define DB_H

#include <stdint.h>

#include <library/core/co_api.h>
#include <library/database/db_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void db_create(void);
void db_destroy(void);

int32_t db_get_pipeline_id(void);

vector_t db_load_pipeline_configs(void);
vector_t db_load_pipeline_configs_by_type(uint32_t pipeline_type);
pipeline_resource_t db_load_pipeline_resource_by_id(int32_t pipeline_id);
vector_t db_load_pipeline_vertex_input_binding_descriptions_by_id(int32_t pipeline_id);
vector_t db_load_pipeline_vertex_input_attribute_descriptions_by_id(int32_t pipeline_id);
vector_t db_load_pipeline_descriptor_pool_sizes_by_id(int32_t pipeline_id);
vector_t db_load_pipeline_descriptor_set_layout_bindings_by_id(int32_t pipeline_id);

void db_store_pipeline_config(char const *pipeline_name, uint32_t pipeline_type, uint32_t pipeline_auto_create);
void db_store_pipeline_resource(int32_t pipeline_id, uint8_t *vertex_shader, uint64_t vertex_shader_size, uint8_t *fragment_shader, uint64_t fragment_shader_size, uint8_t *compute_shader, uint64_t compute_shader_size);

void db_store_pipeline_vertex_input_binding_description(int32_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t stride, uint32_t input_rate);
void db_store_pipeline_vertex_input_attribute_description(int32_t pipeline_id, char const *attribute_name, uint32_t location, uint32_t binding, uint32_t format, uint32_t offset);
void db_store_pipeline_descriptor_pool_size(int32_t pipeline_id, uint32_t descriptor_type, uint32_t descriptor_count);
void db_store_pipeline_descriptor_set_layout_bindings(int32_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t descriptor_type, uint32_t descriptor_count, uint32_t stage_flags, uint8_t auto_buffer);

void db_destroy_pipeline_configs(vector_t *pipeline_configs);
void db_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource);
void db_destroy_vertex_input_binding_descriptions(vector_t *vertex_input_binding_descriptions);
void db_destroy_vertex_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions);
void db_destroy_descriptor_pool_sizes(vector_t *descriptor_pool_sizes);
void db_destroy_descriptor_set_layout_bindings(vector_t *descriptor_set_layout_bindings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DB_H
