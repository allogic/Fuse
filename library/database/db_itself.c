#include <stdio.h>

#include <library/database/db_itself.h>
#include <library/database/db_macros.h>
#include <library/database/db_config.h>

#include <sqlite3/sqlite3.h>

static sqlite3 *s_db_handle = 0;

void db_create(void) {
  SQL_CHECK(sqlite3_open(DB_FILE_PATH, &s_db_handle));
}
void db_destroy(void) {
  SQL_CHECK(sqlite3_close(s_db_handle));
}

int32_t db_get_pipeline_id(void) {
  int32_t last_id = 1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT seq FROM sqlite_sequence WHERE name = 'PIPELINE_CONFIGS'\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    last_id = sqlite3_column_int(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return last_id;
}

vector_t db_load_pipeline_configs(void) {
  vector_t pipeline_configs = vector_create(sizeof(pipeline_config_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME, TYPE, AUTO_CREATE FROM PIPELINE_CONFIGS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_config_t pipeline_config = {0};

    uint32_t id = (uint32_t)sqlite3_column_int(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint8_t auto_create = (uint8_t)sqlite3_column_int(stmt, 3);

    pipeline_config.id = id;
    pipeline_config.name = string_create_from(name);
    pipeline_config.type = type;
    pipeline_config.auto_create = auto_create;

    vector_push(&pipeline_configs, &pipeline_config);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_configs;
}
vector_t db_load_pipeline_configs_by_type(uint32_t pipeline_type) {
  vector_t pipeline_configs = vector_create(sizeof(pipeline_config_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME, AUTO_CREATE FROM PIPELINE_CONFIGS WHERE TYPE = %d\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_config_t pipeline_config = {0};

    uint32_t id = (uint32_t)sqlite3_column_int(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint8_t auto_create = (uint8_t)sqlite3_column_int(stmt, 2);

    pipeline_config.id = id;
    pipeline_config.name = string_create_from(name);
    pipeline_config.type = pipeline_type;
    pipeline_config.auto_create = auto_create;

    vector_push(&pipeline_configs, &pipeline_config);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_configs;
}
pipeline_resource_t db_load_pipeline_resource_by_id(int32_t pipeline_id) {
  pipeline_resource_t pipeline_resource = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PR.ID, PR.VERTEX_SHADER, PR.FRAGMENT_SHADER, PR.COMPUTE_SHADER\n");
  string_appendf(&sql, "FROM PIPELINE_CONFIGS AS PC\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_RESOURCES AS PR\n");
  string_appendf(&sql, "ON PC.ID = PR.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PC.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int32_t id = sqlite3_column_int(stmt, 0);
    uint8_t *vertex_shader = (uint8_t *)sqlite3_column_blob(stmt, 1);
    uint8_t *fragment_shader = (uint8_t *)sqlite3_column_blob(stmt, 2);
    uint8_t *compute_shader = (uint8_t *)sqlite3_column_blob(stmt, 3);
    uint64_t vertex_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 1);
    uint64_t fragment_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 2);
    uint64_t compute_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 3);

    pipeline_resource.id = id;
    pipeline_resource.pipeline_id = pipeline_id;
    pipeline_resource.vertex_shader = vector_create_from(sizeof(uint8_t), vertex_shader, vertex_shader_size);
    pipeline_resource.fragment_shader = vector_create_from(sizeof(uint8_t), fragment_shader, fragment_shader_size);
    pipeline_resource.compute_shader = vector_create_from(sizeof(uint8_t), compute_shader, compute_shader_size);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_resource;
}
vector_t db_load_pipeline_vertex_input_binding_descriptions_by_id(int32_t pipeline_id) {
  vector_t pipeline_vertex_input_binding_descriptions = vector_create(sizeof(pipeline_vertex_input_binding_description_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PVIBD.ID, PVIBD.BINDING_NAME, PVIBD.BINDING, PVIBD.STRIDE, PVIBD.INPUT_RATE\n");
  string_appendf(&sql, "FROM PIPELINE_CONFIGS AS PC\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDING_DESCRIPTIONS AS PVIBD\n");
  string_appendf(&sql, "ON PC.ID = PVIBD.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PC.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_vertex_input_binding_description_t pipeline_vertex_input_binding_description = {0};

    int32_t id = sqlite3_column_int(stmt, 0);
    char const *binding_name = sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t stride = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t input_rate = (uint32_t)sqlite3_column_int(stmt, 4);

    pipeline_vertex_input_binding_description.id = id;
    pipeline_vertex_input_binding_description.pipeline_id = pipeline_id;
    pipeline_vertex_input_binding_description.binding_name = string_create_from(binding_name);
    pipeline_vertex_input_binding_description.binding = binding;
    pipeline_vertex_input_binding_description.stride = stride;
    pipeline_vertex_input_binding_description.input_rate = input_rate;

    vector_push(&pipeline_vertex_input_binding_descriptions, &pipeline_vertex_input_binding_description);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_vertex_input_binding_descriptions;
}
vector_t db_load_pipeline_vertex_input_attribute_descriptions_by_id(int32_t pipeline_id) {
  vector_t pipeline_vertex_input_attribute_descriptions = vector_create(sizeof(pipeline_vertex_input_attribute_description_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PVIAD.ID, PVIAD.ATTRIBUTE_NAME, PVIAD.LOCATION, PVIAD.BINDING, PVIAD.FORMAT, PVIAD.OFFSET\n");
  string_appendf(&sql, "FROM PIPELINE_CONFIGS AS PC\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS AS PVIAD\n");
  string_appendf(&sql, "ON PC.ID = PVIAD.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PC.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_vertex_input_attribute_description_t pipeline_vertex_input_attribute_description = {0};

    int32_t id = sqlite3_column_int(stmt, 0);
    char const *attribute_name = sqlite3_column_text(stmt, 1);
    uint32_t location = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t format = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t offset = (uint32_t)sqlite3_column_int(stmt, 5);

    pipeline_vertex_input_attribute_description.id = id;
    pipeline_vertex_input_attribute_description.pipeline_id = pipeline_id;
    pipeline_vertex_input_attribute_description.attribute_name = string_create_from(attribute_name);
    pipeline_vertex_input_attribute_description.location = location;
    pipeline_vertex_input_attribute_description.binding = binding;
    pipeline_vertex_input_attribute_description.format = format;
    pipeline_vertex_input_attribute_description.offset = offset;

    vector_push(&pipeline_vertex_input_attribute_descriptions, &pipeline_vertex_input_attribute_description);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_vertex_input_attribute_descriptions;
}
vector_t db_load_pipeline_descriptor_pool_sizes_by_id(int32_t pipeline_id) {
  vector_t pipeline_descriptor_pool_sizes = vector_create(sizeof(pipeline_descriptor_pool_size_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PDPS.ID, PDPS.TYPE, PDPS.COUNT\n");
  string_appendf(&sql, "FROM PIPELINE_CONFIGS AS PC\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_DESCRIPTOR_POOL_SIZES AS PDPS\n");
  string_appendf(&sql, "ON PC.ID = PDPS.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PC.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_descriptor_pool_size_t pipeline_descriptor_pool_size = {0};

    int32_t id = sqlite3_column_int(stmt, 0);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 1);
    uint32_t count = (uint32_t)sqlite3_column_int(stmt, 2);

    pipeline_descriptor_pool_size.id = id;
    pipeline_descriptor_pool_size.pipeline_id = pipeline_id;
    pipeline_descriptor_pool_size.type = type;
    pipeline_descriptor_pool_size.count = count;

    vector_push(&pipeline_descriptor_pool_sizes, &pipeline_descriptor_pool_size);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_descriptor_pool_sizes;
}
vector_t db_load_pipeline_descriptor_set_layout_bindings_by_id(int32_t pipeline_id) {
  vector_t pipeline_descriptor_set_layout_bindings = vector_create(sizeof(pipeline_descriptor_set_layout_binding_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PDSLB.ID, PDSLB.BINDING_NAME, PDSLB.BINDING, PDSLB.DESCRIPTOR_TYPE, PDSLB.DESCRIPTOR_COUNT, PDSLB.STAGE_FLAGS, PDSLB.AUTO_BUFFER\n");
  string_appendf(&sql, "FROM PIPELINE_CONFIGS AS PC\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_DESCRIPTOR_SET_LAYOUT_BINDINGS AS PDSLB\n");
  string_appendf(&sql, "ON PC.ID = PDSLB.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PC.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_descriptor_set_layout_binding_t pipeline_descriptor_set_layout_binding = {0};

    int32_t id = sqlite3_column_int(stmt, 0);
    char const *binding_name = (char const *)sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t descriptor_type = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t descriptor_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t stage_flags = (uint32_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_buffer = (uint8_t)sqlite3_column_int(stmt, 6);

    pipeline_descriptor_set_layout_binding.id = id;
    pipeline_descriptor_set_layout_binding.pipeline_id = pipeline_id;
    pipeline_descriptor_set_layout_binding.binding_name = string_create_from(binding_name);
    pipeline_descriptor_set_layout_binding.binding = binding;
    pipeline_descriptor_set_layout_binding.descriptor_type = descriptor_type;
    pipeline_descriptor_set_layout_binding.descriptor_count = descriptor_count;
    pipeline_descriptor_set_layout_binding.stage_flags = stage_flags;
    pipeline_descriptor_set_layout_binding.auto_buffer = auto_buffer;

    vector_push(&pipeline_descriptor_set_layout_bindings, &pipeline_descriptor_set_layout_binding);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_descriptor_set_layout_bindings;
}

void db_store_pipeline_config(char const *pipeline_name, uint32_t pipeline_type, uint32_t pipeline_auto_create) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_CONFIGS (NAME, TYPE, AUTO_CREATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_type);
  sqlite3_bind_int(stmt, 3, pipeline_auto_create);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
void db_store_pipeline_resource(int32_t pipeline_id, uint8_t *vertex_shader, uint64_t vertex_shader_size, uint8_t *fragment_shader, uint64_t fragment_shader_size, uint8_t *compute_shader, uint64_t compute_shader_size) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_RESOURCES (PIPELINE_ID, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int(stmt, 1, pipeline_id);
  sqlite3_bind_blob(stmt, 2, vertex_shader, (int32_t)vertex_shader_size, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 3, fragment_shader, (int32_t)fragment_shader_size, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 4, compute_shader, (int32_t)compute_shader_size, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}

void db_store_pipeline_vertex_input_binding_description(int32_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t stride, uint32_t input_rate) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDING_DESCRIPTIONS (PIPELINE_ID, BINDING_NAME, BINDING, STRIDE, INPUT_RATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, binding);
  sqlite3_bind_int(stmt, 4, stride);
  sqlite3_bind_int(stmt, 5, input_rate);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
void db_store_pipeline_vertex_input_attribute_description(int32_t pipeline_id, char const *attribute_name, uint32_t location, uint32_t binding, uint32_t format, uint32_t offset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS (PIPELINE_ID, ATTRIBUTE_NAME, LOCATION, BINDING, FORMAT, OFFSET)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, attribute_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, location);
  sqlite3_bind_int(stmt, 4, binding);
  sqlite3_bind_int(stmt, 5, format);
  sqlite3_bind_int(stmt, 6, offset);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
void db_store_pipeline_descriptor_pool_size(int32_t pipeline_id, uint32_t descriptor_type, uint32_t descriptor_count) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_DESCRIPTOR_POOL_SIZES (PIPELINE_ID, TYPE, COUNT)\n");
  string_appendf(&sql, "VALUES (?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (TYPE) DO UPDATE\n");
  string_appendf(&sql, "SET COUNT = PIPELINE_DESCRIPTOR_POOL_SIZES.COUNT + EXCLUDED.COUNT\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int(stmt, 1, pipeline_id);
  sqlite3_bind_int(stmt, 2, descriptor_type);
  sqlite3_bind_int(stmt, 3, descriptor_count);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
void db_store_pipeline_descriptor_set_layout_bindings(int32_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t descriptor_type, uint32_t descriptor_count, uint32_t stage_flags, uint8_t auto_buffer) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_DESCRIPTOR_SET_LAYOUT_BINDINGS (PIPELINE_ID, BINDING_NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (BINDING) DO UPDATE\n");
  string_appendf(&sql, "SET STAGE_FLAGS = PIPELINE_DESCRIPTOR_SET_LAYOUT_BINDINGS.STAGE_FLAGS | EXCLUDED.STAGE_FLAGS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_db_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, binding);
  sqlite3_bind_int(stmt, 4, descriptor_type);
  sqlite3_bind_int(stmt, 5, descriptor_count);
  sqlite3_bind_int(stmt, 6, stage_flags);
  sqlite3_bind_int(stmt, 7, auto_buffer);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}

void db_destroy_pipeline_configs(vector_t *pipeline_configs) {
  uint64_t pipeline_config_index = 0;
  uint64_t pipeline_config_count = vector_count(pipeline_configs);

  while (pipeline_config_index < pipeline_config_count) {
    pipeline_config_t *pipeline_config = (pipeline_config_t *)vector_at(pipeline_configs, pipeline_config_index);

    string_destroy(&pipeline_config->name);

    pipeline_config_index++;
  }

  vector_destroy(pipeline_configs);
}
void db_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource) {
  vector_destroy(&pipeline_resource->vertex_shader);
  vector_destroy(&pipeline_resource->fragment_shader);
  vector_destroy(&pipeline_resource->compute_shader);
}
void db_destroy_vertex_input_binding_descriptions(vector_t *vertex_input_binding_descriptions) {
  uint64_t vertex_input_binding_description_index = 0;
  uint64_t vertex_input_binding_description_count = vector_count(vertex_input_binding_descriptions);

  while (vertex_input_binding_description_index < vertex_input_binding_description_count) {
    pipeline_vertex_input_binding_description_t *vertex_input_binding_description = (pipeline_vertex_input_binding_description_t *)vector_at(vertex_input_binding_descriptions, vertex_input_binding_description_index);

    string_destroy(&vertex_input_binding_description->binding_name);

    vertex_input_binding_description_index++;
  }

  vector_destroy(vertex_input_binding_descriptions);
}
void db_destroy_vertex_input_attribute_descriptions(vector_t *vertex_input_attribute_descriptions) {
  uint64_t vertex_input_attribute_description_index = 0;
  uint64_t vertex_input_attribute_description_count = vector_count(vertex_input_attribute_descriptions);

  while (vertex_input_attribute_description_index < vertex_input_attribute_description_count) {
    pipeline_vertex_input_attribute_description_t *vertex_input_attribute_description = (pipeline_vertex_input_attribute_description_t *)vector_at(vertex_input_attribute_descriptions, vertex_input_attribute_description_index);

    string_destroy(&vertex_input_attribute_description->attribute_name);

    vertex_input_attribute_description_index++;
  }

  vector_destroy(vertex_input_attribute_descriptions);
}
void db_destroy_descriptor_pool_sizes(vector_t *descriptor_pool_sizes) {
  vector_destroy(descriptor_pool_sizes);
}
void db_destroy_descriptor_set_layout_bindings(vector_t *descriptor_set_layout_bindings) {
  uint64_t vertex_input_descriptor_set_layout_index = 0;
  uint64_t vertex_input_descriptor_set_layout_count = vector_count(descriptor_set_layout_bindings);

  while (vertex_input_descriptor_set_layout_index < vertex_input_descriptor_set_layout_count) {
    pipeline_vertex_input_attribute_description_t *vertex_input_attribute_description = (pipeline_vertex_input_attribute_description_t *)vector_at(descriptor_set_layout_bindings, vertex_input_descriptor_set_layout_index);

    string_destroy(&vertex_input_attribute_description->attribute_name);

    vertex_input_descriptor_set_layout_index++;
  }

  vector_destroy(descriptor_set_layout_bindings);
}
