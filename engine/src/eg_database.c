#include <engine/eg_pch.h>

static void eg_database_create_swapchain_tables(void);
static void eg_database_create_renderer_tables(void);
static void eg_database_create_pipeline_tables(void);
static void eg_database_create_scene_tables(void);
static void eg_database_create_model_tables(void);
static void eg_database_create_graph_tables(void);

static void eg_database_create_default_swapchain_assets(void);
static void eg_database_create_default_renderer_assets(void);
static void eg_database_create_default_pipeline_assets(void);
static void eg_database_create_default_scene_assets(void);
static void eg_database_create_default_model_assets(void);
static void eg_database_create_default_graph_assets(void);

static int64_t eg_database_get_sequence_index_by_name(char const *table_name);

static sqlite3 *s_database_handle = 0;

void eg_database_create(void) {
  EG_SQL_CHECK(sqlite3_open("engine.db", &s_database_handle));

  eg_database_create_swapchain_tables();
  eg_database_create_renderer_tables();
  eg_database_create_pipeline_tables();
  eg_database_create_scene_tables();
  eg_database_create_model_tables();
  eg_database_create_graph_tables();

  eg_database_create_default_swapchain_assets();
  eg_database_create_default_renderer_assets();
  eg_database_create_default_pipeline_assets();
  eg_database_create_default_scene_assets();
  eg_database_create_default_model_assets();
  eg_database_create_default_graph_assets();
}
void eg_database_destroy(void) {
  EG_SQL_CHECK(sqlite3_close(s_database_handle));
}

eg_scene_asset_id_t eg_database_load_default_scene_asset_id(void) {
  eg_scene_asset_id_t scene_asset_id = -1;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SCENE_ASSET.ID\n");
  eg_string_appendf(sql, "FROM SCENE_ASSET\n");
  eg_string_appendf(sql, "WHERE SCENE_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    scene_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return scene_asset_id;
}
eg_swapchain_asset_id_t eg_database_load_default_swapchain_asset_id(void) {
  eg_swapchain_asset_id_t swapchain_asset_id = -1;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID\n");
  eg_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");
  eg_string_appendf(sql, "WHERE SWAPCHAIN_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    swapchain_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return swapchain_asset_id;
}
eg_renderer_asset_id_t eg_database_load_default_renderer_asset_id(void) {
  eg_renderer_asset_id_t renderer_asset_id = -1;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT RENDERER_ASSET.ID\n");
  eg_string_appendf(sql, "FROM RENDERER_ASSET\n");
  eg_string_appendf(sql, "WHERE RENDERER_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return renderer_asset_id;
}

eg_vector_t *eg_database_load_all_swapchain_assets(void) {
  eg_vector_t *swapchain_assets = eg_vector_create(sizeof(eg_swapchain_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID, SWAPCHAIN_ASSET.NAME, SWAPCHAIN_ASSET.IMAGE_COUNT, SWAPCHAIN_ASSET.IS_DEFAULT\n");
  eg_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_swapchain_asset_t swapchain_asset = {0};

    eg_swapchain_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    swapchain_asset.id = id;
    strcpy(swapchain_asset.name, name);
    swapchain_asset.image_count = image_count;
    swapchain_asset.is_default = is_default;

    eg_vector_push(swapchain_assets, &swapchain_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return swapchain_assets;
}
eg_swapchain_asset_t eg_database_load_swapchain_asset_by_id(eg_swapchain_asset_id_t swapchain_asset_id) {
  eg_swapchain_asset_t swapchain_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID, SWAPCHAIN_ASSET.NAME, SWAPCHAIN_ASSET.IMAGE_COUNT, SWAPCHAIN_ASSET.IS_DEFAULT\n");
  eg_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");
  eg_string_appendf(sql, "WHERE SWAPCHAIN_ASSET.ID = %lld\n", swapchain_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_swapchain_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    swapchain_asset.id = id;
    strcpy(swapchain_asset.name, name);
    swapchain_asset.image_count = image_count;
    swapchain_asset.is_default = is_default;
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return swapchain_asset;
}

eg_vector_t *eg_database_load_all_renderer_assets(void) {
  eg_vector_t *renderer_assets = eg_vector_create(sizeof(eg_renderer_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT RENDERER_ASSET.ID, RENDERER_ASSET.NAME, RENDERER_ASSET.FRAMES_IN_FLIGHT, RENDERER_ASSET.IS_DEFAULT\n");
  eg_string_appendf(sql, "FROM RENDERER_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_renderer_asset_t renderer_asset = {0};

    eg_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    renderer_asset.id = id;
    strcpy(renderer_asset.name, name);
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;

    eg_vector_push(renderer_assets, &renderer_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return renderer_assets;
}
eg_renderer_asset_t eg_database_load_renderer_asset_by_id(eg_renderer_asset_id_t renderer_asset_id) {
  eg_renderer_asset_t renderer_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT RENDERER_ASSET.ID, RENDERER_ASSET.NAME, RENDERER_ASSET.FRAMES_IN_FLIGHT, RENDERER_ASSET.IS_DEFAULT\n");
  eg_string_appendf(sql, "FROM RENDERER_ASSET\n");
  eg_string_appendf(sql, "WHERE RENDERER_ASSET.ID = %lld\n", renderer_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    renderer_asset.id = id;
    strcpy(renderer_asset.name, name);
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return renderer_asset;
}

uint64_t eg_database_load_vertex_input_binding_count_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_vertex_input_binding_count = 0;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT COUNT(*) AS CNT\n");
  eg_string_appendf(sql, "FROM PIPELINE_VERTEX_INPUT_BINDING\n");
  eg_string_appendf(sql, "WHERE PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_vertex_input_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_vertex_input_binding_count;
}
uint64_t eg_database_load_descriptor_binding_count_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_descriptor_binding_count = 0;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT COUNT(*) AS CNT\n");
  eg_string_appendf(sql, "FROM PIPELINE_DESCRIPTOR_BINDING\n");
  eg_string_appendf(sql, "WHERE PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_descriptor_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_descriptor_binding_count;
}

eg_vector_t *eg_database_load_all_pipeline_assets(void) {
  eg_vector_t *pipeline_assets = eg_vector_create(sizeof(eg_pipeline_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_asset_t pipeline_asset = {0};

    eg_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    eg_pipeline_type_t type = (eg_pipeline_type_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    eg_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_assets;
}
eg_vector_t *eg_database_load_all_pipeline_assets_by_type(eg_pipeline_type_t pipeline_type) {
  eg_vector_t *pipeline_assets = eg_vector_create(sizeof(eg_pipeline_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  eg_string_appendf(sql, "WHERE TYPE = %u\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_asset_t pipeline_asset = {0};

    eg_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    eg_pipeline_type_t type = (eg_pipeline_type_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    eg_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_assets;
}
eg_pipeline_asset_t eg_database_load_pipeline_asset_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  eg_pipeline_asset_t pipeline_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    eg_pipeline_type_t type = (eg_pipeline_type_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_asset;
}
eg_pipeline_resource_t eg_database_load_pipeline_resource_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  eg_pipeline_resource_t pipeline_resource = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_RESOURCE.ID, PIPELINE_RESOURCE.VERTEX_SHADER_FILE_PATH, PIPELINE_RESOURCE.FRAGMENT_SHADER_FILE_PATH, PIPELINE_RESOURCE.COMPUTE_SHADER_FILE_PATH\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  eg_string_appendf(sql, "LEFT JOIN PIPELINE_RESOURCE\n");
  eg_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_RESOURCE.PIPELINE_ASSET_ID\n");
  eg_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *vertex_shader_file_path = sqlite3_column_text(stmt, 1);
    char const *fragment_shader_file_path = sqlite3_column_text(stmt, 2);
    char const *compute_shader_file_path = sqlite3_column_text(stmt, 3);

    pipeline_resource.id = id;
    pipeline_resource.pipeline_asset_id = pipeline_asset_id;
    strcpy(pipeline_resource.vertex_shader_file_path, vertex_shader_file_path);
    strcpy(pipeline_resource.fragment_shader_file_path, fragment_shader_file_path);
    strcpy(pipeline_resource.compute_shader_file_path, compute_shader_file_path);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_resource;
}
eg_vector_t *eg_database_load_all_pipeline_vertex_input_bindings_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  eg_vector_t *pipeline_vertex_input_bindings = eg_vector_create(sizeof(eg_pipeline_vertex_input_binding_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_VERTEX_INPUT_BINDING.ID, PIPELINE_VERTEX_INPUT_BINDING.NAME, PIPELINE_VERTEX_INPUT_BINDING.LOCATION, PIPELINE_VERTEX_INPUT_BINDING.SIZE, PIPELINE_VERTEX_INPUT_BINDING.COMPONENT_COUNT, PIPELINE_VERTEX_INPUT_BINDING.FORMAT, PIPELINE_VERTEX_INPUT_BINDING.INPUT_RATE\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  eg_string_appendf(sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDING\n");
  eg_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ASSET_ID\n");
  eg_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    eg_pipeline_vertex_input_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t location = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t size = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t component_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t format = (uint32_t)sqlite3_column_int(stmt, 5);
    uint32_t input_rate = (uint32_t)sqlite3_column_int(stmt, 6);

    pipeline_vertex_input_binding.id = id;
    pipeline_vertex_input_binding.pipeline_asset_id = pipeline_asset_id;
    strcpy(pipeline_vertex_input_binding.name, name);
    pipeline_vertex_input_binding.location = location;
    pipeline_vertex_input_binding.size = size;
    pipeline_vertex_input_binding.component_count = component_count;
    pipeline_vertex_input_binding.format = format;
    pipeline_vertex_input_binding.input_rate = input_rate;

    eg_vector_push(pipeline_vertex_input_bindings, &pipeline_vertex_input_binding);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_vertex_input_bindings;
}
eg_vector_t *eg_database_load_all_pipeline_descriptor_bindings_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  eg_vector_t *pipeline_descriptor_bindings = eg_vector_create(sizeof(eg_pipeline_descriptor_binding_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_DESCRIPTOR_BINDING.ID, PIPELINE_DESCRIPTOR_BINDING.NAME, PIPELINE_DESCRIPTOR_BINDING.BINDING, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_TYPE, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_COUNT, PIPELINE_DESCRIPTOR_BINDING.STAGE_FLAGS, PIPELINE_DESCRIPTOR_BINDING.AUTO_BUFFER\n");
  eg_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  eg_string_appendf(sql, "LEFT JOIN PIPELINE_DESCRIPTOR_BINDING\n");
  eg_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ASSET_ID\n");
  eg_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_pipeline_descriptor_binding_t pipeline_descriptor_binding = {0};

    eg_pipeline_descriptor_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = (char const *)sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t descriptor_type = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t descriptor_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t stage_flags = (uint32_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_buffer = (uint8_t)sqlite3_column_int(stmt, 6);

    pipeline_descriptor_binding.id = id;
    pipeline_descriptor_binding.pipeline_asset_id = pipeline_asset_id;
    strcpy(pipeline_descriptor_binding.name, name);
    pipeline_descriptor_binding.binding = binding;
    pipeline_descriptor_binding.descriptor_type = descriptor_type;
    pipeline_descriptor_binding.descriptor_count = descriptor_count;
    pipeline_descriptor_binding.stage_flags = stage_flags;
    pipeline_descriptor_binding.auto_buffer = auto_buffer;

    eg_vector_push(pipeline_descriptor_bindings, &pipeline_descriptor_binding);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_descriptor_bindings;
}

eg_vector_t *eg_database_load_all_model_assets(void) {
  eg_vector_t *model_assets = eg_vector_create(sizeof(eg_model_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT MODEL_ASSET.ID, MODEL_ASSET.NAME\n");
  eg_string_appendf(sql, "FROM MODEL_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_model_asset_t model_asset = {0};

    eg_model_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    model_asset.id = id;
    strcpy(model_asset.name, name);

    eg_vector_push(model_assets, &model_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return model_assets;
}
eg_model_resource_t eg_database_load_model_resource_by_id(eg_model_asset_id_t model_asset_id) {
  eg_model_resource_t model_resource = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT MODEL_RESOURCE.ID, MODEL_RESOURCE.MODEL_FILE_PATH\n");
  eg_string_appendf(sql, "FROM MODEL_ASSET\n");
  eg_string_appendf(sql, "LEFT JOIN MODEL_RESOURCE\n");
  eg_string_appendf(sql, "ON MODEL_ASSET.ID = MODEL_RESOURCE.MODEL_ASSET_ID\n");
  eg_string_appendf(sql, "WHERE MODEL_ASSET.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_model_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *model_file_path = sqlite3_column_text(stmt, 1);

    model_resource.id = id;
    model_resource.model_asset_id = model_asset_id;
    strcpy(model_resource.model_file_path, model_file_path);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return model_resource;
}
eg_vector_t *eg_database_load_all_model_meshes_by_id(eg_model_asset_id_t model_asset_id) {
  eg_vector_t *model_meshes = eg_vector_create(sizeof(eg_model_mesh_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT MODEL_MESH.ID, MODEL_MESH.NAME\n");
  eg_string_appendf(sql, "FROM MODEL_ASSET\n");
  eg_string_appendf(sql, "LEFT JOIN MODEL_MESH\n");
  eg_string_appendf(sql, "ON MODEL_ASSET.ID = MODEL_MESH.MODEL_ASSET_ID\n");
  eg_string_appendf(sql, "WHERE MODEL_ASSET.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_model_mesh_t model_mesh = {0};

    eg_model_mesh_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    model_mesh.id = id;
    model_mesh.model_asset_id = model_asset_id;
    strcpy(model_mesh.name, name);

    eg_vector_push(model_meshes, &model_mesh);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return model_meshes;
}

eg_vector_t *eg_database_load_all_mesh_primitives_by_id(eg_model_mesh_id_t model_mesh_id) {
  eg_vector_t *mesh_primitives = eg_vector_create(sizeof(eg_mesh_primitive_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT MESH_PRIMITIVE.ID, MESH_PRIMITIVE.NAME\n");
  eg_string_appendf(sql, "FROM MODEL_MESH\n");
  eg_string_appendf(sql, "LEFT JOIN MESH_PRIMITIVE\n");
  eg_string_appendf(sql, "ON MODEL_MESH.ID = MESH_PRIMITIVE.MODEL_MESH_ID\n");
  eg_string_appendf(sql, "WHERE MODEL_MESH.ID = %lld\n", model_mesh_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_mesh_primitive_t mesh_primitive = {0};

    eg_mesh_primitive_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    mesh_primitive.id = id;
    mesh_primitive.model_mesh_id = model_mesh_id;
    strcpy(mesh_primitive.name, name);

    eg_vector_push(mesh_primitives, &mesh_primitive);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return mesh_primitives;
}
eg_vector_t *eg_database_load_all_mesh_attributes_by_id(eg_mesh_primitive_id_t mesh_primitive_id) {
  eg_vector_t *mesh_attributes = eg_vector_create(sizeof(eg_mesh_attribute_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT MESH_ATTRIBUTE.ID, MESH_ATTRIBUTE.NAME, MESH_ATTRIBUTE.TYPE, MESH_ATTRIBUTE.COUNT\n");
  eg_string_appendf(sql, "FROM MESH_PRIMITIVE\n");
  eg_string_appendf(sql, "LEFT JOIN MESH_ATTRIBUTE\n");
  eg_string_appendf(sql, "ON MESH_PRIMITIVE.ID = MESH_ATTRIBUTE.MESH_PRIMITIVE_ID\n");
  eg_string_appendf(sql, "WHERE MESH_PRIMITIVE.ID = %lld\n", mesh_primitive_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_mesh_attribute_t mesh_attribute = {0};

    eg_mesh_attribute_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = sqlite3_column_int(stmt, 2);
    uint64_t count = sqlite3_column_int64(stmt, 3);

    mesh_attribute.id = id;
    mesh_attribute.mesh_primitive_id = mesh_primitive_id;
    strcpy(mesh_attribute.name, name);
    mesh_attribute.type = type;
    mesh_attribute.count = count;

    eg_vector_push(mesh_attributes, &mesh_attribute);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return mesh_attributes;
}

eg_attribute_buffer_t eg_database_load_attribute_buffer_by_id(eg_mesh_attribute_id_t mesh_attribute_id) {
  eg_attribute_buffer_t attribute_buffer = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT ATTRIBUTE_BUFFER.ID, ATTRIBUTE_BUFFER.DATA\n");
  eg_string_appendf(sql, "FROM MESH_ATTRIBUTE\n");
  eg_string_appendf(sql, "LEFT JOIN ATTRIBUTE_BUFFER\n");
  eg_string_appendf(sql, "ON MESH_ATTRIBUTE.ID = ATTRIBUTE_BUFFER.MESH_ATTRIBUTE_ID\n");
  eg_string_appendf(sql, "WHERE MESH_ATTRIBUTE.ID = %lld\n", mesh_attribute_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_attribute_data_id_t id = sqlite3_column_int64(stmt, 0);
    uint8_t const *data = sqlite3_column_blob(stmt, 1);
    uint64_t data_size = sqlite3_column_bytes(stmt, 1);

    attribute_buffer.id = id;
    attribute_buffer.mesh_attribute_id = mesh_attribute_id;
    attribute_buffer.data = eg_heap_alloc(data_size, 0, data);
    attribute_buffer.data_size = data_size;
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return attribute_buffer;
}

eg_vector_t *eg_database_load_all_scene_assets(void) {
  eg_vector_t *scene_assets = eg_vector_create(sizeof(eg_scene_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SCENE_ASSET.ID, SCENE_ASSET.NAME\n");
  eg_string_appendf(sql, "FROM SCENE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_scene_asset_t scene_asset = {0};

    eg_scene_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    scene_asset.id = id;
    strcpy(scene_asset.name, name);

    eg_vector_push(scene_assets, &scene_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return scene_assets;
}
eg_scene_asset_t eg_database_load_scene_asset_by_id(eg_scene_asset_id_t scene_asset_id) {
  eg_scene_asset_t scene_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT SCENE_ASSET.ID, SCENE_ASSET.NAME\n");
  eg_string_appendf(sql, "FROM SCENE_ASSET\n");
  eg_string_appendf(sql, "WHERE SCENE_ASSET.ID = %lld\n", scene_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_scene_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    scene_asset.id = id;
    strcpy(scene_asset.name, name);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return scene_asset;
}

eg_graph_asset_t eg_database_load_graph_asset_by_id(eg_graph_asset_id_t graph_asset_id) {
  eg_graph_asset_t graph_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT GRAPH_ASSET.ID, GRAPH_ASSET.NAME\n");
  eg_string_appendf(sql, "FROM GRAPH_ASSET\n");
  eg_string_appendf(sql, "WHERE GRAPH_ASSET.ID = %lld\n", graph_asset_id);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    graph_asset.id = id;
    strcpy(graph_asset.name, name);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return graph_asset;
}

void eg_database_store_swapchain_asset(eg_swapchain_asset_t *swapchain_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO SWAPCHAIN_ASSET (NAME, IMAGE_COUNT)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "IMAGE_COUNT = EXCLUDED.IMAGE_COUNT,\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, swapchain_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, swapchain_asset->image_count);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (swapchain_asset->id == 0) {
    swapchain_asset->id = eg_database_get_sequence_index_by_name("SWAPCHAIN_ASSET");
  }
}

void eg_database_store_renderer_asset(eg_renderer_asset_t *renderer_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO RENDERER_ASSET (NAME, FRAMES_IN_FLIGHT)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "FRAMES_IN_FLIGHT = EXCLUDED.FRAMES_IN_FLIGHT\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, renderer_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, renderer_asset->frames_in_flight);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (renderer_asset->id == 0) {
    renderer_asset->id = eg_database_get_sequence_index_by_name("RENDERER_ASSET");
  }
}

void eg_database_store_pipeline_asset(eg_pipeline_asset_t *pipeline_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO PIPELINE_ASSET (NAME, TYPE, LINK_INDEX, AUTO_CREATE_PIPELINE, AUTO_CREATE_VERTEX_INPUT_BUFFER, AUTO_LINK_DESCRIPTOR_BINDINGS, INTERLEAVED_VERTEX_INPUT_BUFFER)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "TYPE = EXCLUDED.TYPE,\n");
  eg_string_appendf(sql, "LINK_INDEX = EXCLUDED.LINK_INDEX,\n");
  eg_string_appendf(sql, "AUTO_CREATE_PIPELINE = EXCLUDED.AUTO_CREATE_PIPELINE,\n");
  eg_string_appendf(sql, "AUTO_CREATE_VERTEX_INPUT_BUFFER = EXCLUDED.AUTO_CREATE_VERTEX_INPUT_BUFFER,\n");
  eg_string_appendf(sql, "AUTO_LINK_DESCRIPTOR_BINDINGS = EXCLUDED.AUTO_LINK_DESCRIPTOR_BINDINGS,\n");
  eg_string_appendf(sql, "INTERLEAVED_VERTEX_INPUT_BUFFER = EXCLUDED.INTERLEAVED_VERTEX_INPUT_BUFFER\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_asset->type);
  sqlite3_bind_int(stmt, 3, pipeline_asset->link_index);
  sqlite3_bind_int(stmt, 4, pipeline_asset->auto_create_pipeline);
  sqlite3_bind_int(stmt, 5, pipeline_asset->auto_create_vertex_input_buffer);
  sqlite3_bind_int(stmt, 6, pipeline_asset->auto_link_descriptor_bindings);
  sqlite3_bind_int(stmt, 7, pipeline_asset->interleaved_vertex_input_buffer);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (pipeline_asset->id == 0) {
    pipeline_asset->id = eg_database_get_sequence_index_by_name("PIPELINE_ASSET");
  }
}
void eg_database_store_pipeline_resource(eg_pipeline_resource_t *pipeline_resource) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO PIPELINE_RESOURCE (PIPELINE_ASSET_ID, VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH, COMPUTE_SHADER_FILE_PATH)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (PIPELINE_ASSET_ID) DO UPDATE SET\n");
  eg_string_appendf(sql, "VERTEX_SHADER_FILE_PATH = EXCLUDED.VERTEX_SHADER_FILE_PATH,\n");
  eg_string_appendf(sql, "FRAGMENT_SHADER_FILE_PATH = EXCLUDED.FRAGMENT_SHADER_FILE_PATH,\n");
  eg_string_appendf(sql, "COMPUTE_SHADER_FILE_PATH = EXCLUDED.COMPUTE_SHADER_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_resource->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_resource->vertex_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pipeline_resource->fragment_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pipeline_resource->compute_shader_file_path, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (pipeline_resource->id == 0) {
    pipeline_resource->id = eg_database_get_sequence_index_by_name("PIPELINE_RESOURCE");
  }
}
void eg_database_store_pipeline_vertex_input_binding(eg_pipeline_vertex_input_binding_t *pipeline_vertex_input_binding) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDING (PIPELINE_ASSET_ID, NAME, LOCATION, SIZE, COMPONENT_COUNT, FORMAT, INPUT_RATE)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (PIPELINE_ASSET_ID, LOCATION) DO UPDATE SET\n");
  eg_string_appendf(sql, "INPUT_RATE = EXCLUDED.INPUT_RATE\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_vertex_input_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_vertex_input_binding->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_vertex_input_binding->location);
  sqlite3_bind_int(stmt, 4, pipeline_vertex_input_binding->size);
  sqlite3_bind_int(stmt, 5, pipeline_vertex_input_binding->component_count);
  sqlite3_bind_int(stmt, 6, pipeline_vertex_input_binding->format);
  sqlite3_bind_int(stmt, 7, pipeline_vertex_input_binding->input_rate);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (pipeline_vertex_input_binding->id == 0) {
    pipeline_vertex_input_binding->id = eg_database_get_sequence_index_by_name("PIPELINE_VERTEX_INPUT_BINDING");
  }
}
void eg_database_store_pipeline_descriptor_binding(eg_pipeline_descriptor_binding_t *pipeline_descriptor_binding) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO PIPELINE_DESCRIPTOR_BINDING (PIPELINE_ASSET_ID, NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (PIPELINE_ASSET_ID, BINDING) DO UPDATE SET\n");
  eg_string_appendf(sql, "STAGE_FLAGS = EXCLUDED.STAGE_FLAGS | PIPELINE_DESCRIPTOR_BINDING.STAGE_FLAGS,\n");
  eg_string_appendf(sql, "AUTO_BUFFER = EXCLUDED.AUTO_BUFFER\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_descriptor_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_descriptor_binding->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_descriptor_binding->binding);
  sqlite3_bind_int(stmt, 4, pipeline_descriptor_binding->descriptor_type);
  sqlite3_bind_int(stmt, 5, pipeline_descriptor_binding->descriptor_count);
  sqlite3_bind_int(stmt, 6, pipeline_descriptor_binding->stage_flags);
  sqlite3_bind_int(stmt, 7, pipeline_descriptor_binding->auto_buffer);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (pipeline_descriptor_binding->id == 0) {
    pipeline_descriptor_binding->id = eg_database_get_sequence_index_by_name("PIPELINE_DESCRIPTOR_BINDING");
  }
}

void eg_database_store_model_asset(eg_model_asset_t *model_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO MODEL_ASSET (NAME)\n");
  eg_string_appendf(sql, "VALUES (?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, model_asset->name, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (model_asset->id == 0) {
    model_asset->id = eg_database_get_sequence_index_by_name("MODEL_ASSET");
  }
}
void eg_database_store_model_resource(eg_model_resource_t *model_resource) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO MODEL_RESOURCE (MODEL_ASSET_ID, MODEL_FILE_PATH)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (MODEL_ASSET_ID) DO UPDATE SET\n");
  eg_string_appendf(sql, "MODEL_FILE_PATH = EXCLUDED.MODEL_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_resource->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_resource->model_file_path, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (model_resource->id == 0) {
    model_resource->id = eg_database_get_sequence_index_by_name("MODEL_RESOURCE");
  }
}
void eg_database_store_model_mesh(eg_model_mesh_t *model_mesh) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO MODEL_MESH (MODEL_ASSET_ID, NAME)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (MODEL_ASSET_ID) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_mesh->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_mesh->name, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (model_mesh->id == 0) {
    model_mesh->id = eg_database_get_sequence_index_by_name("MODEL_MESH");
  }
}

void eg_database_store_mesh_primitive(eg_mesh_primitive_t *mesh_primitive) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO MESH_PRIMITIVE (MODEL_MESH_ID, NAME)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_primitive->model_mesh_id);
  sqlite3_bind_text(stmt, 2, mesh_primitive->name, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (mesh_primitive->id == 0) {
    mesh_primitive->id = eg_database_get_sequence_index_by_name("MESH_PRIMITIVE");
  }
}
void eg_database_store_mesh_attribute(eg_mesh_attribute_t *mesh_attribute) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO MESH_ATTRIBUTE (MESH_PRIMITIVE_ID, NAME, TYPE, COUNT)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_attribute->mesh_primitive_id);
  sqlite3_bind_text(stmt, 2, mesh_attribute->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, mesh_attribute->type);
  sqlite3_bind_int64(stmt, 4, mesh_attribute->count);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (mesh_attribute->id == 0) {
    mesh_attribute->id = eg_database_get_sequence_index_by_name("MESH_ATTRIBUTE");
  }
}

void eg_database_store_attribute_buffer(eg_attribute_buffer_t *attribute_buffer) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO ATTRIBUTE_BUFFER (MESH_ATTRIBUTE_ID, DATA)\n");
  eg_string_appendf(sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, attribute_buffer->mesh_attribute_id);
  sqlite3_bind_blob(stmt, 2, attribute_buffer->data, (int32_t)attribute_buffer->data_size, SQLITE_STATIC);
  sqlite3_bind_int64(stmt, 3, attribute_buffer->data_size);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (attribute_buffer->id == 0) {
    attribute_buffer->id = eg_database_get_sequence_index_by_name("ATTRIBUTE_BUFFER");
  }
}

void eg_database_store_scene(eg_scene_asset_t *scene_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO SCENE_ASSET (NAME)\n");
  eg_string_appendf(sql, "VALUES (?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, scene_asset->name, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (scene_asset->id == 0) {
    scene_asset->id = eg_database_get_sequence_index_by_name("SCENE_ASSET");
  }
}

void eg_database_store_graph_asset(eg_graph_asset_t *graph_asset) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO GRAPH_ASSET (NAME)\n");
  eg_string_appendf(sql, "VALUES (?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, graph_asset->name, -1, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (graph_asset->id == 0) {
    graph_asset->id = eg_database_get_sequence_index_by_name("GRAPH_ASSET");
  }
}

void eg_database_destroy_swapchain_asset(eg_swapchain_asset_t *swapchain_asset) {
  // TODO
}
void eg_database_destroy_swapchain_assets(eg_vector_t *swapchain_assets) {
  // TODO

  eg_vector_destroy(swapchain_assets);
}

void eg_database_destroy_renderer_asset(eg_renderer_asset_t *renderer_asset) {
  // TODO
}
void eg_database_destroy_renderer_assets(eg_vector_t *renderer_assets) {
  // TODO

  eg_vector_destroy(renderer_assets);
}

void eg_database_destroy_pipeline_asset(eg_pipeline_asset_t *pipeline_asset) {
  // TODO
}
void eg_database_destroy_pipeline_assets(eg_vector_t *pipeline_assets) {
  // TODO

  eg_vector_destroy(pipeline_assets);
}
void eg_database_destroy_pipeline_resource(eg_pipeline_resource_t *pipeline_resource) {
  // TODO
}
void eg_database_destroy_pipeline_vertex_input_bindings(eg_vector_t *pipeline_vertex_input_bindings) {
  // TODO

  eg_vector_destroy(pipeline_vertex_input_bindings);
}
void eg_database_destroy_pipeline_descriptor_bindings(eg_vector_t *pipeline_descriptor_bindings) {
  // TODO

  eg_vector_destroy(pipeline_descriptor_bindings);
}

void eg_database_destroy_model_assets(eg_vector_t *model_assets) {
  // TODO

  eg_vector_destroy(model_assets);
}
void eg_database_destroy_model_resource(eg_model_resource_t *model_resource) {
  // TODO
}
void eg_database_destroy_model_meshes(eg_vector_t *model_meshes) {
  // TODO

  eg_vector_destroy(model_meshes);
}

void eg_database_destroy_mesh_primitives(eg_vector_t *mesh_primitives) {
  // TODO

  eg_vector_destroy(mesh_primitives);
}
void eg_database_destroy_mesh_attributes(eg_vector_t *mesh_attributes) {
  // TODO

  eg_vector_destroy(mesh_attributes);
}

void eg_database_destroy_attribute_buffer(eg_attribute_buffer_t *attribute_buffer) {
  eg_heap_free(attribute_buffer->data);
}

void eg_database_destroy_scene_asset(eg_scene_asset_t *scene_asset) {
  // TODO
}
void eg_database_destroy_scene_assets(eg_vector_t *scene_assets) {
  // TODO

  eg_vector_destroy(scene_assets);
}

void eg_database_destroy_graph_assets(eg_vector_t *graph_assets) {
  // TODO

  eg_vector_destroy(graph_assets);
}

static void eg_database_create_swapchain_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS SWAPCHAIN_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "IMAGE_COUNT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "IS_DEFAULT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}
static void eg_database_create_renderer_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS RENDERER_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "FRAMES_IN_FLIGHT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "IS_DEFAULT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}
static void eg_database_create_pipeline_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS PIPELINE_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "TYPE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "LINK_INDEX INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "AUTO_CREATE_PIPELINE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "AUTO_CREATE_VERTEX_INPUT_BUFFER INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "AUTO_LINK_DESCRIPTOR_BINDINGS INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "INTERLEAVED_VERTEX_INPUT_BUFFER INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS PIPELINE_RESOURCE (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "PIPELINE_ASSET_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "VERTEX_SHADER_FILE_PATH VARCHAR(255),\n");
    eg_string_appendf(sql, "FRAGMENT_SHADER_FILE_PATH VARCHAR(255),\n");
    eg_string_appendf(sql, "COMPUTE_SHADER_FILE_PATH VARCHAR(255),\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (PIPELINE_ASSET_ID) REFERENCES PIPELINE_ASSET(ID)\n");
    eg_string_appendf(sql, "UNIQUE (PIPELINE_ASSET_ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS PIPELINE_VERTEX_INPUT_BINDING (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "PIPELINE_ASSET_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "LOCATION INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "SIZE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "COMPONENT_COUNT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "FORMAT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "INPUT_RATE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (PIPELINE_ASSET_ID) REFERENCES PIPELINE_ASSET(ID)\n");
    eg_string_appendf(sql, "UNIQUE (PIPELINE_ASSET_ID, LOCATION)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS PIPELINE_DESCRIPTOR_BINDING (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "PIPELINE_ASSET_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "BINDING INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "DESCRIPTOR_TYPE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "DESCRIPTOR_COUNT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "STAGE_FLAGS INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "AUTO_BUFFER INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (PIPELINE_ASSET_ID) REFERENCES PIPELINE_ASSET(ID)\n");
    eg_string_appendf(sql, "UNIQUE (PIPELINE_ASSET_ID, BINDING)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}
static void eg_database_create_scene_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS SCENE_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "IS_DEFAULT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}
static void eg_database_create_model_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS MODEL_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS MODEL_RESOURCE (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "MODEL_ASSET_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "MODEL_FILE_PATH VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (MODEL_ASSET_ID) REFERENCES MODEL_ASSET(ID)\n");
    eg_string_appendf(sql, "UNIQUE (MODEL_ASSET_ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS MODEL_MESH (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "MODEL_ASSET_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (MODEL_ASSET_ID) REFERENCES MODEL_ASSET(ID)\n");
    eg_string_appendf(sql, "UNIQUE (MODEL_ASSET_ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS MESH_PRIMITIVE (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "MODEL_MESH_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (MODEL_MESH_ID) REFERENCES MODEL_MESH(ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS MESH_ATTRIBUTE (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "MESH_PRIMITIVE_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "TYPE INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "COUNT INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (MESH_PRIMITIVE_ID) REFERENCES MESH_PRIMITIVE(ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }

  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS ATTRIBUTE_BUFFER (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "MESH_ATTRIBUTE_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "DATA BLOB NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "FOREIGN KEY (MESH_ATTRIBUTE_ID) REFERENCES MESH_ATTRIBUTE(ID)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}
static void eg_database_create_graph_tables(void) {
  {
    eg_string_t *sql = eg_string_create();

    eg_string_appendf(sql, "CREATE TABLE IF NOT EXISTS GRAPH_ASSET (\n");
    eg_string_appendf(sql, "ID INTEGER PRIMARY KEY AUTOINCREMENT,\n");
    eg_string_appendf(sql, "NAME VARCHAR(255) NOT NULL,\n");
    eg_string_appendf(sql, "CREATED_AT DATETIME DEFAULT CURRENT_TIMESTAMP,\n");
    eg_string_appendf(sql, "UNIQUE (NAME)\n");
    eg_string_appendf(sql, ")\n");

    sqlite3_stmt *stmt = 0;

    EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

    EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);

    eg_string_destroy(sql);
  }
}

static void eg_database_create_default_swapchain_assets(void) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO SWAPCHAIN_ASSET (NAME, IMAGE_COUNT, IS_DEFAULT) VALUES\n");
  eg_string_appendf(sql, "('default', 1, 1)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "IMAGE_COUNT = EXCLUDED.IMAGE_COUNT,\n");
  eg_string_appendf(sql, "IS_DEFAULT = EXCLUDED.IS_DEFAULT\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);
}
static void eg_database_create_default_renderer_assets(void) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO RENDERER_ASSET (NAME, FRAMES_IN_FLIGHT, IS_DEFAULT) VALUES\n");
  eg_string_appendf(sql, "('default', 1, 1)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "FRAMES_IN_FLIGHT = EXCLUDED.FRAMES_IN_FLIGHT,\n");
  eg_string_appendf(sql, "IS_DEFAULT = EXCLUDED.IS_DEFAULT\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);
}
static void eg_database_create_default_pipeline_assets(void) {
  // TODO
}
static void eg_database_create_default_scene_assets(void) {
  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "INSERT INTO SCENE_ASSET (NAME, IS_DEFAULT) VALUES\n");
  eg_string_appendf(sql, "('default', 1)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "IS_DEFAULT = EXCLUDED.IS_DEFAULT\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);
}
static void eg_database_create_default_model_assets(void) {
  // TODO
}
static void eg_database_create_default_graph_assets(void) {
  // TODO
}

static int64_t eg_database_get_sequence_index_by_name(char const *table_name) {
  int64_t sequence_id = 1;

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT seq\n");
  eg_string_appendf(sql, "FROM sqlite_sequence\n");
  eg_string_appendf(sql, "WHERE name = '%s'\n", table_name);

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    sequence_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return sequence_id;
}
