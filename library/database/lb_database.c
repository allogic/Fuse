#include <library/lb_pch.h>

#include <library/database/lb_database.h>

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
    swapchain_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    int32_t depth_format = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    swapchain_asset.id = id;
    swapchain_asset.name = heap_alloc(name_size, 1, name);
    swapchain_asset.name_size = name_size;
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
    renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t frames_in_flight = (uint32_t)sqlite3_column_int(stmt, 2);

    uint64_t name_size = strlen(name) + 1;

    renderer_asset.id = id;
    renderer_asset.name = heap_alloc(name_size, 1, name);
    renderer_asset.name_size = name_size;
    renderer_asset.frames_in_flight = frames_in_flight;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return renderer_asset;
}

uint64_t database_load_vertex_input_binding_count_by_id(pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_vertex_input_binding_count = 0;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT COUNT(*) AS CNT\n");
  string_appendf(&sql, "FROM PIPELINE_VERTEX_INPUT_BINDINGS AS PVIB\n");
  string_appendf(&sql, "WHERE PVIB.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_vertex_input_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_vertex_input_binding_count;
}
uint64_t database_load_descriptor_binding_count_by_id(pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_descriptor_binding_count = 0;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT COUNT(*) AS CNT\n");
  string_appendf(&sql, "FROM PIPELINE_DESCRIPTOR_BINDINGS AS PDB\n");
  string_appendf(&sql, "WHERE PDB.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_descriptor_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_descriptor_binding_count;
}

vector_t database_load_pipeline_assets(void) {
  vector_t pipeline_assets = vector_create(sizeof(pipeline_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT * FROM PIPELINE_ASSETS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_asset_t pipeline_asset = {0};

    pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    uint64_t name_size = strlen(name) + 1;

    pipeline_asset.id = id;
    pipeline_asset.name = heap_alloc(name_size, 1, name);
    pipeline_asset.name_size = name_size;
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    vector_push(&pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_assets;
}
vector_t database_load_pipeline_assets_by_type(uint32_t pipeline_type) {
  vector_t pipeline_assets = vector_create(sizeof(pipeline_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT * FROM PIPELINE_ASSETS WHERE TYPE = %d\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_asset_t pipeline_asset = {0};

    pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    uint64_t name_size = strlen(name) + 1;

    pipeline_asset.id = id;
    pipeline_asset.name = heap_alloc(name_size, 1, name);
    pipeline_asset.name_size = name_size;
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    vector_push(&pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_assets;
}
pipeline_resource_t database_load_pipeline_resource_by_id(pipeline_asset_id_t pipeline_asset_id) {
  pipeline_resource_t pipeline_resource = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PR.ID, PR.VERTEX_SHADER_FILE_PATH, PR.FRAGMENT_SHADER_FILE_PATH, PR.COMPUTE_SHADER_FILE_PATH\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_RESOURCES AS PR\n");
  string_appendf(&sql, "ON PA.ID = PR.PIPELINE_ASSET_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *vertex_shader_file_path = sqlite3_column_text(stmt, 1);
    char const *fragment_shader_file_path = sqlite3_column_text(stmt, 2);
    char const *compute_shader_file_path = sqlite3_column_text(stmt, 3);

    uint64_t vertex_shader_file_path_size = vertex_shader_file_path ? strlen(vertex_shader_file_path) + 1 : 0;
    uint64_t fragment_shader_file_path_size = fragment_shader_file_path ? strlen(fragment_shader_file_path) + 1 : 0;
    uint64_t compute_shader_file_path_size = compute_shader_file_path ? strlen(compute_shader_file_path) + 1 : 0;

    pipeline_resource.id = id;
    pipeline_resource.pipeline_asset_id = pipeline_asset_id;
    pipeline_resource.vertex_shader_file_path = vertex_shader_file_path ? heap_alloc(vertex_shader_file_path_size, 1, vertex_shader_file_path) : 0;
    pipeline_resource.fragment_shader_file_path = fragment_shader_file_path ? heap_alloc(fragment_shader_file_path_size, 1, fragment_shader_file_path) : 0;
    pipeline_resource.compute_shader_file_path = compute_shader_file_path ? heap_alloc(compute_shader_file_path_size, 1, compute_shader_file_path) : 0;
    pipeline_resource.vertex_shader_file_path_size = vertex_shader_file_path_size;
    pipeline_resource.fragment_shader_file_path_size = fragment_shader_file_path_size;
    pipeline_resource.compute_shader_file_path_size = compute_shader_file_path_size;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_resource;
}
vector_t database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset_id_t pipeline_asset_id) {
  vector_t pipeline_vertex_input_bindings = vector_create(sizeof(pipeline_vertex_input_binding_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PVIB.ID, PVIB.BINDING_NAME, PVIB.LOCATION, PVIB.SIZE, PVIB.COMPONENT_COUNT, PVIB.FORMAT, PVIB.INPUT_RATE\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDINGS AS PVIB\n");
  string_appendf(&sql, "ON PA.ID = PVIB.PIPELINE_ASSET_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    pipeline_vertex_input_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *binding_name = sqlite3_column_text(stmt, 1);
    uint32_t location = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t size = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t component_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t format = (uint32_t)sqlite3_column_int(stmt, 5);
    uint32_t input_rate = (uint32_t)sqlite3_column_int(stmt, 6);

    uint64_t binding_name_size = binding_name ? strlen(binding_name) + 1 : 0;

    pipeline_vertex_input_binding.id = id;
    pipeline_vertex_input_binding.pipeline_asset_id = pipeline_asset_id;
    pipeline_vertex_input_binding.binding_name = heap_alloc(binding_name_size, 1, binding_name);
    pipeline_vertex_input_binding.binding_name_size = binding_name_size;
    pipeline_vertex_input_binding.location = location;
    pipeline_vertex_input_binding.size = size;
    pipeline_vertex_input_binding.component_count = component_count;
    pipeline_vertex_input_binding.format = format;
    pipeline_vertex_input_binding.input_rate = input_rate;

    vector_push(&pipeline_vertex_input_bindings, &pipeline_vertex_input_binding);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return pipeline_vertex_input_bindings;
}
vector_t database_load_pipeline_descriptor_bindings_by_id(pipeline_asset_id_t pipeline_asset_id) {
  vector_t pipeline_descriptor_bindings = vector_create(sizeof(pipeline_descriptor_binding_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT PDB.ID, PDB.BINDING_NAME, PDB.BINDING, PDB.DESCRIPTOR_TYPE, PDB.DESCRIPTOR_COUNT, PDB.STAGE_FLAGS, PDB.AUTO_BUFFER\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "LEFT JOIN PIPELINE_DESCRIPTOR_BINDINGS AS PDB\n");
  string_appendf(&sql, "ON PA.ID = PDB.PIPELINE_ASSET_ID\n");
  string_appendf(&sql, "WHERE PA.ID = %lld\n", pipeline_asset_id);

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

    uint64_t binding_name_size = binding_name ? strlen(binding_name) + 1 : 0;

    pipeline_descriptor_binding.id = id;
    pipeline_descriptor_binding.pipeline_asset_id = pipeline_asset_id;
    pipeline_descriptor_binding.binding_name = heap_alloc(binding_name_size, 1, binding_name);
    pipeline_descriptor_binding.binding_name_size = binding_name_size;
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

vector_t database_load_model_assets(void) {
  vector_t model_assets = vector_create(sizeof(pipeline_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME FROM MODEL_ASSETS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    model_asset_t model_asset = {0};

    model_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    model_asset.id = id;
    model_asset.name = heap_alloc(name_size, 1, name);
    model_asset.name_size = name_size;

    vector_push(&model_assets, &model_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return model_assets;
}
// TODO: implement the rest of model stuff..

void database_store_pipeline_asset(pipeline_asset_t *pipeline_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_ASSETS (NAME, TYPE, LINK_INDEX, AUTO_CREATE_PIPELINE, AUTO_CREATE_VERTEX_INPUT_BUFFER, AUTO_LINK_DESCRIPTOR_BINDINGS, INTERLEAVED_VERTEX_INPUT_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  string_appendf(&sql, "ID = PIPELINE_ASSETS.ID,\n");
  string_appendf(&sql, "NAME = PIPELINE_ASSETS.NAME\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_asset->type);
  sqlite3_bind_int(stmt, 3, pipeline_asset->link_index);
  sqlite3_bind_int(stmt, 4, pipeline_asset->auto_create_pipeline);
  sqlite3_bind_int(stmt, 5, pipeline_asset->auto_create_vertex_input_buffer);
  sqlite3_bind_int(stmt, 6, pipeline_asset->auto_link_descriptor_bindings);
  sqlite3_bind_int(stmt, 7, pipeline_asset->interleaved_vertex_input_buffer);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  pipeline_asset->id = database_get_sequence_index_by_name("PIPELINE_ASSETS");
}
void database_store_pipeline_resource(pipeline_resource_t *pipeline_resource) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_RESOURCES (PIPELINE_ASSET_ID, VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH, COMPUTE_SHADER_FILE_PATH)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID) DO UPDATE SET\n");
  string_appendf(&sql, "ID = PIPELINE_RESOURCES.ID,\n");
  string_appendf(&sql, "PIPELINE_ASSET_ID = PIPELINE_RESOURCES.PIPELINE_ASSET_ID\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_resource->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_resource->vertex_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pipeline_resource->fragment_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pipeline_resource->compute_shader_file_path, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  pipeline_resource->id = database_get_sequence_index_by_name("PIPELINE_RESOURCES");
}
void database_store_pipeline_vertex_input_binding(pipeline_vertex_input_binding_t *pipeline_vertex_input_binding) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDINGS (PIPELINE_ASSET_ID, BINDING_NAME, LOCATION, SIZE, COMPONENT_COUNT, FORMAT, INPUT_RATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID, LOCATION) DO UPDATE SET\n");
  string_appendf(&sql, "ID = PIPELINE_VERTEX_INPUT_BINDINGS.ID,\n");
  string_appendf(&sql, "PIPELINE_ASSET_ID = PIPELINE_VERTEX_INPUT_BINDINGS.PIPELINE_ASSET_ID\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_vertex_input_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_vertex_input_binding->binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_vertex_input_binding->location);
  sqlite3_bind_int(stmt, 4, pipeline_vertex_input_binding->size);
  sqlite3_bind_int(stmt, 5, pipeline_vertex_input_binding->component_count);
  sqlite3_bind_int(stmt, 6, pipeline_vertex_input_binding->format);
  sqlite3_bind_int(stmt, 7, pipeline_vertex_input_binding->input_rate);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  pipeline_vertex_input_binding->id = database_get_sequence_index_by_name("PIPELINE_VERTEX_INPUT_BINDINGS");
}
void database_store_pipeline_descriptor_binding(pipeline_descriptor_binding_t *pipeline_descriptor_binding) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_DESCRIPTOR_BINDINGS (PIPELINE_ASSET_ID, BINDING_NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID, BINDING) DO UPDATE SET\n");
  string_appendf(&sql, "ID = PIPELINE_DESCRIPTOR_BINDINGS.ID,\n");
  string_appendf(&sql, "PIPELINE_ASSET_ID = PIPELINE_DESCRIPTOR_BINDINGS.PIPELINE_ASSET_ID,\n");
  string_appendf(&sql, "STAGE_FLAGS = PIPELINE_DESCRIPTOR_BINDINGS.STAGE_FLAGS | EXCLUDED.STAGE_FLAGS\n"); // TODO: how to handle this case..

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_descriptor_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_descriptor_binding->binding_name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_descriptor_binding->binding);
  sqlite3_bind_int(stmt, 4, pipeline_descriptor_binding->descriptor_type);
  sqlite3_bind_int(stmt, 5, pipeline_descriptor_binding->descriptor_count);
  sqlite3_bind_int(stmt, 6, pipeline_descriptor_binding->stage_flags);
  sqlite3_bind_int(stmt, 7, pipeline_descriptor_binding->auto_buffer);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  pipeline_descriptor_binding->id = database_get_sequence_index_by_name("PIPELINE_DESCRIPTOR_BINDINGS");
}

void database_store_model_asset(model_asset_t *model_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_ASSETS (NAME)\n");
  string_appendf(&sql, "VALUES (?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  string_appendf(&sql, "ID = MODEL_ASSETS.ID,\n");
  string_appendf(&sql, "NAME = MODEL_ASSETS.NAME\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, model_asset->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  model_asset->id = database_get_sequence_index_by_name("MODEL_ASSETS");
}
void database_store_model_resource(model_resource_t *model_resource) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_RESOURCES (MODEL_ASSET_ID, MODEL_FILE_PATH)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (MODEL_ASSET_ID) DO UPDATE SET\n");
  string_appendf(&sql, "ID = MODEL_RESOURCES.ID,\n");
  string_appendf(&sql, "MODEL_ASSET_ID = MODEL_RESOURCES.MODEL_ASSET_ID\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_resource->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_resource->model_file_path, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  model_resource->id = database_get_sequence_index_by_name("MODEL_RESOURCES");
}
void database_store_model_mesh(model_mesh_t *model_mesh) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_MESHES (MODEL_ASSET_ID)\n");
  string_appendf(&sql, "VALUES (?)\n");
  string_appendf(&sql, "ON CONFLICT (MODEL_ASSET_ID) DO UPDATE SET\n");
  string_appendf(&sql, "ID = MODEL_MESHES.ID,\n");
  string_appendf(&sql, "MODEL_ASSET_ID = MODEL_MESHES.MODEL_ASSET_ID\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_mesh->model_asset_id);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  model_mesh->id = database_get_sequence_index_by_name("MODEL_MESHES");
}
void database_store_model_primitive(model_primitive_t *model_primitive) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_PRIMITIVES (MODEL_MESH_ID)\n");
  string_appendf(&sql, "VALUES (?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_primitive->model_mesh_id);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  model_primitive->id = database_get_sequence_index_by_name("MODEL_PRIMITIVES");
}
void database_store_model_attribute(model_attribute_t *model_attribute) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_ATTRIBUTES (MODEL_PRIMITIVE_ID, NAME, TYPE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_attribute->model_primitive_id);
  sqlite3_bind_text(stmt, 2, model_attribute->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, model_attribute->type);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  model_attribute->id = database_get_sequence_index_by_name("MODEL_ATTRIBUTES");
}

void database_destroy_swapchain_asset(swapchain_asset_t *swapchain_asset) {
  heap_free(swapchain_asset->name);
}

void database_destroy_renderer_asset(renderer_asset_t *renderer_asset) {
  heap_free(renderer_asset->name);
}

void database_destroy_pipeline_assets(vector_t *pipeline_assets) {
  uint64_t pipeline_asset_index = 0;
  uint64_t pipeline_asset_count = vector_count(pipeline_assets);

  while (pipeline_asset_index < pipeline_asset_count) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(pipeline_assets, pipeline_asset_index);

    heap_free(pipeline_asset->name);

    pipeline_asset_index++;
  }

  vector_destroy(pipeline_assets);
}
void database_destroy_pipeline_resource(pipeline_resource_t *pipeline_resource) {
  if (pipeline_resource->vertex_shader_file_path) {
    heap_free(pipeline_resource->vertex_shader_file_path);
  }

  if (pipeline_resource->fragment_shader_file_path) {
    heap_free(pipeline_resource->fragment_shader_file_path);
  }

  if (pipeline_resource->compute_shader_file_path) {
    heap_free(pipeline_resource->compute_shader_file_path);
  }
}
void database_destroy_pipeline_vertex_input_bindings(vector_t *vertex_input_bindings) {
  uint64_t vertex_input_binding_index = 0;
  uint64_t vertex_input_binding_count = vector_count(vertex_input_bindings);

  while (vertex_input_binding_index < vertex_input_binding_count) {
    pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(vertex_input_bindings, vertex_input_binding_index);

    heap_free(vertex_input_binding->binding_name);

    vertex_input_binding_index++;
  }

  vector_destroy(vertex_input_bindings);
}
void database_destroy_pipeline_descriptor_bindings(vector_t *descriptor_bindings) {
  uint64_t descriptor_binding_index = 0;
  uint64_t descriptor_binding_count = vector_count(descriptor_bindings);

  while (descriptor_binding_index < descriptor_binding_count) {
    pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(descriptor_bindings, descriptor_binding_index);

    heap_free(pipeline_descriptor_binding->binding_name);

    descriptor_binding_index++;
  }

  vector_destroy(descriptor_bindings);
}

void database_destroy_model_assets(vector_t *model_assets) {
  uint64_t model_asset_index = 0;
  uint64_t model_asset_count = vector_count(model_assets);

  while (model_asset_index < model_asset_count) {
    model_asset_t *model_asset = (model_asset_t *)vector_at(model_assets, model_asset_index);

    heap_free(model_asset->name);

    model_asset_index++;
  }

  vector_destroy(model_assets);
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
