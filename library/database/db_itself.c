#include <library/lb_pch.h>

#include <library/core/co_api.h>

#include <library/database/db_itself.h>

#include <sqlite3/sqlite3.h>

static int32_t database_get_sequence_index_by_name(char const *table_name);

static sqlite3 *s_database_handle = 0;

void database_create(void) {
  SQL_CHECK(sqlite3_open(DB_FILE_PATH, &s_database_handle));
}
void database_destroy(void) {
  SQL_CHECK(sqlite3_close(s_database_handle));
}

swapchain_asset_t database_load_swapchain_default_asset(void) {
  swapchain_asset_t swapchain_asset = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT SA.ID, SA.NAME, SA.IMAGE_COUNT, SA.DEPTH_FORMAT\n");
  string_appendf(&sql, "FROM SWAPCHAIN_ASSETS AS SA\n");
  string_appendf(&sql, "WHERE SA.NAME = 'default'\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    swapchain_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    int32_t depth_format = sqlite3_column_int(stmt, 3);

    swapchain_asset.id = id;
    swapchain_asset.name = string_create_from(name);
    swapchain_asset.image_count = image_count;
    swapchain_asset.depth_format = depth_format;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return swapchain_asset;
}
renderer_asset_t database_load_renderer_default_asset(void) {
  renderer_asset_t renderer_asset = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT RA.ID, RA.NAME, RA.FRAMES_IN_FLIGHT\n");
  string_appendf(&sql, "FROM RENDERER_ASSETS AS RA\n");
  string_appendf(&sql, "WHERE RA.NAME = 'default'\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    renderer_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t frames_in_flight = (uint32_t)sqlite3_column_int(stmt, 2);

    renderer_asset.id = id;
    renderer_asset.name = string_create_from(name);
    renderer_asset.frames_in_flight = frames_in_flight;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return renderer_asset;
}

vector_t database_load_pipeline_assets(void) {
  vector_t pipeline_assets = vector_create(sizeof(pipeline_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME, TYPE, LINK_INDEX, AUTO_CREATE FROM PIPELINE_ASSETS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_asset_t pipeline_asset = {0};

    pipeline_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create = (uint8_t)sqlite3_column_int(stmt, 4);

    pipeline_asset.id = id;
    pipeline_asset.name = string_create_from(name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create = auto_create;

    vector_push(&pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_assets;
}
vector_t database_load_pipeline_assets_by_type(uint32_t pipeline_type) {
  vector_t pipeline_assets = vector_create(sizeof(pipeline_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME, LINK_INDEX, AUTO_CREATE FROM PIPELINE_ASSETS WHERE TYPE = %d\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_asset_t pipeline_asset = {0};

    pipeline_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 2);
    uint8_t auto_create = (uint8_t)sqlite3_column_int(stmt, 3);

    pipeline_asset.id = id;
    pipeline_asset.name = string_create_from(name);
    pipeline_asset.type = pipeline_type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create = auto_create;

    vector_push(&pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_assets;
}
pipeline_resource_t database_load_pipeline_resource_by_id(pipeline_id_t pipeline_id) {
  pipeline_resource_t pipeline_resource = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PR.ID, PR.VERTEX_SHADER_FILE_PATH, PR.FRAGMENT_SHADER_FILE_PATH, PR.COMPUTE_SHADER_FILE_PATH, PR.VERTEX_SHADER, PR.FRAGMENT_SHADER, PR.COMPUTE_SHADER\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_RESOURCES AS PR\n");
  string_appendf(&sql, "ON PA.ID = PR.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *vertex_shader_file_path = sqlite3_column_text(stmt, 1);
    char const *fragment_shader_file_path = sqlite3_column_text(stmt, 2);
    char const *compute_shader_file_path = sqlite3_column_text(stmt, 3);
    uint8_t *vertex_shader = (uint8_t *)sqlite3_column_blob(stmt, 4);
    uint8_t *fragment_shader = (uint8_t *)sqlite3_column_blob(stmt, 5);
    uint8_t *compute_shader = (uint8_t *)sqlite3_column_blob(stmt, 6);
    uint64_t vertex_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 4);
    uint64_t fragment_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 5);
    uint64_t compute_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 6);

    pipeline_resource.id = id;
    pipeline_resource.pipeline_id = pipeline_id;
    pipeline_resource.vertex_shader_file_path = string_create_from(vertex_shader_file_path);
    pipeline_resource.fragment_shader_file_path = string_create_from(fragment_shader_file_path);
    pipeline_resource.compute_shader_file_path = string_create_from(compute_shader_file_path);
    pipeline_resource.vertex_shader = vector_create_from(sizeof(uint8_t), vertex_shader, vertex_shader_size);
    pipeline_resource.fragment_shader = vector_create_from(sizeof(uint8_t), fragment_shader, fragment_shader_size);
    pipeline_resource.compute_shader = vector_create_from(sizeof(uint8_t), compute_shader, compute_shader_size);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_resource;
}
vector_t database_load_pipeline_vertex_input_bindings_by_id(pipeline_id_t pipeline_id) {
  vector_t pipeline_vertex_input_bindings = vector_create(sizeof(pipeline_vertex_input_binding_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PVIB.ID, PVIB.BINDING_NAME, PVIB.BINDING, PVIB.LOCATION, PVIB.STRIDE, PVIB.FORMAT, PVIB.OFFSET, PVIB.INPUT_RATE\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDINGS AS PVIB\n");
  string_appendf(&sql, "ON PA.ID = PVIB.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    pipeline_vertex_input_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *binding_name = sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t location = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t stride = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t format = (uint32_t)sqlite3_column_int(stmt, 5);
    uint32_t offset = (uint32_t)sqlite3_column_int(stmt, 6);
    uint32_t input_rate = (uint32_t)sqlite3_column_int(stmt, 7);

    pipeline_vertex_input_binding.id = id;
    pipeline_vertex_input_binding.pipeline_id = pipeline_id;
    pipeline_vertex_input_binding.binding_name = string_create_from(binding_name);
    pipeline_vertex_input_binding.binding = binding;
    pipeline_vertex_input_binding.location = location;
    pipeline_vertex_input_binding.stride = stride;
    pipeline_vertex_input_binding.format = format;
    pipeline_vertex_input_binding.offset = offset;
    pipeline_vertex_input_binding.input_rate = input_rate;

    vector_push(&pipeline_vertex_input_bindings, &pipeline_vertex_input_binding);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_vertex_input_bindings;
}
vector_t database_load_pipeline_descriptor_bindings_by_id(pipeline_id_t pipeline_id) {
  vector_t pipeline_descriptor_bindings = vector_create(sizeof(pipeline_descriptor_binding_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PDB.ID, PDB.BINDING_NAME, PDB.BINDING, PDB.DESCRIPTOR_TYPE, PDB.DESCRIPTOR_COUNT, PDB.STAGE_FLAGS, PDB.AUTO_BUFFER\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_DESCRIPTOR_BINDINGS AS PDB\n");
  string_appendf(&sql, "ON PA.ID = PDB.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %d\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_descriptor_binding_t pipeline_descriptor_binding = {0};

    pipeline_descriptor_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *binding_name = (char const *)sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t descriptor_type = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t descriptor_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t stage_flags = (uint32_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_buffer = (uint8_t)sqlite3_column_int(stmt, 6);

    pipeline_descriptor_binding.id = id;
    pipeline_descriptor_binding.pipeline_id = pipeline_id;
    pipeline_descriptor_binding.binding_name = string_create_from(binding_name);
    pipeline_descriptor_binding.binding = binding;
    pipeline_descriptor_binding.descriptor_type = descriptor_type;
    pipeline_descriptor_binding.descriptor_count = descriptor_count;
    pipeline_descriptor_binding.stage_flags = stage_flags;
    pipeline_descriptor_binding.auto_buffer = auto_buffer;

    vector_push(&pipeline_descriptor_bindings, &pipeline_descriptor_binding);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_descriptor_bindings;
}

pipeline_id_t database_store_pipeline_asset(char const *pipeline_name, uint32_t pipeline_type, uint32_t pipeline_link_index, uint32_t pipeline_auto_create) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_ASSETS (NAME, TYPE, LINK_INDEX, AUTO_CREATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_type);
  sqlite3_bind_int(stmt, 3, pipeline_link_index);
  sqlite3_bind_int(stmt, 4, pipeline_auto_create);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return database_get_sequence_index_by_name("PIPELINE_ASSETS");
}
pipeline_resource_id_t database_store_pipeline_resource(pipeline_id_t pipeline_id, char const *vertex_shader_file_path, char const *fragment_shader_file_path, char const *compute_shader_file_path, uint8_t *vertex_shader, uint8_t *fragment_shader, uint8_t *compute_shader, uint64_t vertex_shader_size, uint64_t fragment_shader_size, uint64_t compute_shader_size) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_RESOURCES (PIPELINE_ID, VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH, COMPUTE_SHADER_FILE_PATH, VERTEX_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ID) DO UPDATE\n");
  string_appendf(&sql, "SET PIPELINE_ID = PIPELINE_RESOURCES.PIPELINE_ID\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, vertex_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, fragment_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, compute_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 5, vertex_shader, (int32_t)vertex_shader_size, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 6, fragment_shader, (int32_t)fragment_shader_size, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 7, compute_shader, (int32_t)compute_shader_size, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return database_get_sequence_index_by_name("PIPELINE_RESOURCES");
}
pipeline_vertex_input_binding_id_t database_store_pipeline_vertex_input_binding(pipeline_id_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t location, uint32_t stride, uint32_t format, uint32_t offset, uint32_t input_rate) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDINGS (PIPELINE_ID, BINDING_NAME, BINDING, LOCATION, STRIDE, FORMAT, OFFSET, INPUT_RATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ID, LOCATION) DO UPDATE SET\n");
  string_appendf(&sql, "PIPELINE_ID = PIPELINE_VERTEX_INPUT_BINDINGS.PIPELINE_ID,\n");
  string_appendf(&sql, "LOCATION = PIPELINE_VERTEX_INPUT_BINDINGS.LOCATION\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, binding);
  sqlite3_bind_int(stmt, 4, location);
  sqlite3_bind_int(stmt, 5, stride);
  sqlite3_bind_int(stmt, 6, format);
  sqlite3_bind_int(stmt, 7, offset);
  sqlite3_bind_int(stmt, 8, input_rate);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return database_get_sequence_index_by_name("PIPELINE_VERTEX_INPUT_BINDINGS");
}
pipeline_descriptor_binding_id_t database_store_pipeline_descriptor_binding(pipeline_id_t pipeline_id, char const *binding_name, uint32_t binding, uint32_t descriptor_type, uint32_t descriptor_count, uint32_t stage_flags, uint8_t auto_buffer) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_DESCRIPTOR_BINDINGS (PIPELINE_ID, BINDING_NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ID, BINDING) DO UPDATE SET\n");
  string_appendf(&sql, "PIPELINE_ID = PIPELINE_DESCRIPTOR_BINDINGS.PIPELINE_ID,\n");
  string_appendf(&sql, "BINDING = PIPELINE_DESCRIPTOR_BINDINGS.BINDING,\n");
  string_appendf(&sql, "STAGE_FLAGS = PIPELINE_DESCRIPTOR_BINDINGS.STAGE_FLAGS | EXCLUDED.STAGE_FLAGS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_id);
  sqlite3_bind_text(stmt, 2, binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, binding);
  sqlite3_bind_int(stmt, 4, descriptor_type);
  sqlite3_bind_int(stmt, 5, descriptor_count);
  sqlite3_bind_int(stmt, 6, stage_flags);
  sqlite3_bind_int(stmt, 7, auto_buffer);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return database_get_sequence_index_by_name("PIPELINE_DESCRIPTOR_BINDINGS");
}

void database_destroy_swapchain_asset(swapchain_asset_t *swapchain_asset) {
  string_destroy(&swapchain_asset->name);
}
void database_destroy_renderer_asset(renderer_asset_t *renderer_asset) {
  string_destroy(&renderer_asset->name);
}

void database_destroy_pipeline_assets(vector_t *pipeline_assets) {
  uint64_t pipeline_asset_index = 0;
  uint64_t pipeline_asset_count = vector_count(pipeline_assets);

  while (pipeline_asset_index < pipeline_asset_count) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(pipeline_assets, pipeline_asset_index);

    string_destroy(&pipeline_asset->name);

    pipeline_asset_index++;
  }

  vector_destroy(pipeline_assets);
}
void database_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource) {
  string_destroy(&pipeline_resource->vertex_shader_file_path);
  string_destroy(&pipeline_resource->fragment_shader_file_path);
  string_destroy(&pipeline_resource->compute_shader_file_path);

  vector_destroy(&pipeline_resource->vertex_shader);
  vector_destroy(&pipeline_resource->fragment_shader);
  vector_destroy(&pipeline_resource->compute_shader);
}
void database_destroy_pipeline_vertex_input_bindings(vector_t *vertex_input_bindings) {
  uint64_t vertex_input_binding_index = 0;
  uint64_t vertex_input_binding_count = vector_count(vertex_input_bindings);

  while (vertex_input_binding_index < vertex_input_binding_count) {
    pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(vertex_input_bindings, vertex_input_binding_index);

    string_destroy(&vertex_input_binding->binding_name);

    vertex_input_binding_index++;
  }

  vector_destroy(vertex_input_bindings);
}
void database_destroy_pipeline_descriptor_bindings(vector_t *descriptor_bindings) {
  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(descriptor_bindings);

  while (descriptor_binding_index < descriptor_binding_count) {
    pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(descriptor_bindings, descriptor_binding_index);

    string_destroy(&pipeline_descriptor_binding->binding_name);

    descriptor_binding_index++;
  }

  vector_destroy(descriptor_bindings);
}

static int32_t database_get_sequence_index_by_name(char const *table_name) {
  int32_t sequence_id = 1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT seq FROM sqlite_sequence WHERE name = '%s'\n", table_name);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    sequence_id = sqlite3_column_int(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return sequence_id;
}
