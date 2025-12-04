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

eg_vector_t *eg_database_load_all_pipeline_assets(void) {
  eg_vector_t *pipeline_assets = eg_vector_create(sizeof(eg_pipeline_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDING, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.SOURCE, PIPELINE_ASSET.SPIRV\n");
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
    uint8_t auto_link_descriptor_binding = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);
    char const *source = sqlite3_column_text(stmt, 8);
    uint8_t const *spirv = sqlite3_column_blob(stmt, 9);
    uint64_t spirv_size = sqlite3_column_bytes(stmt, 9);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_binding = auto_link_descriptor_binding;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;
    pipeline_asset.source = eg_string_value(source);
    pipeline_asset.spirv = spirv ? eg_heap_alloc(spirv_size, 0, spirv) : 0;
    pipeline_asset.spirv_size = spirv_size;

    eg_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_assets;
}
eg_vector_t *eg_database_load_all_pipeline_assets_by_type(eg_pipeline_type_t pipeline_type) {
  eg_vector_t *pipeline_assets = eg_vector_create(sizeof(eg_pipeline_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.SOURCE, PIPELINE_ASSET.SPIRV\n");
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
    char const *source = sqlite3_column_text(stmt, 8);
    uint8_t const *spirv = sqlite3_column_blob(stmt, 9);
    uint64_t spirv_size = sqlite3_column_bytes(stmt, 9);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_binding = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;
    pipeline_asset.source = eg_string_value(source);
    pipeline_asset.spirv = spirv ? eg_heap_alloc(spirv_size, 0, spirv) : 0;
    pipeline_asset.spirv_size = spirv_size;

    eg_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_assets;
}
eg_pipeline_asset_t eg_database_load_pipeline_asset_by_id(eg_pipeline_asset_id_t pipeline_asset_id) {
  eg_pipeline_asset_t pipeline_asset = {0};

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.SOURCE, PIPELINE_ASSET.SPIRV\n");
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
    char const *source = sqlite3_column_text(stmt, 8);
    uint8_t const *spirv = sqlite3_column_blob(stmt, 9);
    uint64_t spirv_size = sqlite3_column_bytes(stmt, 9);

    pipeline_asset.id = id;
    strcpy(pipeline_asset.name, name);
    pipeline_asset.type = type;
    pipeline_asset.link_index = link_index;
    pipeline_asset.auto_create_pipeline = auto_create_pipeline;
    pipeline_asset.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline_asset.auto_link_descriptor_binding = auto_link_descriptor_bindings;
    pipeline_asset.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;
    pipeline_asset.source = eg_string_value(source);
    pipeline_asset.spirv = spirv ? eg_heap_alloc(spirv_size, 0, spirv) : 0;
    pipeline_asset.spirv_size = spirv_size;
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return pipeline_asset;
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

eg_vector_t *eg_database_load_all_graph_assets(void) {
  eg_vector_t *graph_assets = eg_vector_create(sizeof(eg_graph_asset_t));

  eg_string_t *sql = eg_string_create();

  eg_string_appendf(sql, "SELECT GRAPH_ASSET.ID, GRAPH_ASSET.NAME, GRAPH_ASSET.UNIQUE_NODE_ID, GRAPH_ASSET.UNIQUE_PIN_ID, GRAPH_ASSET.UNIQUE_LINK_ID\n");
  eg_string_appendf(sql, "FROM GRAPH_ASSET\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    eg_graph_asset_t graph_asset = {0};

    eg_scene_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t unique_node_id = sqlite3_column_int(stmt, 2);
    uint32_t unique_pin_id = sqlite3_column_int(stmt, 3);
    uint32_t unique_link_id = sqlite3_column_int(stmt, 4);

    graph_asset.id = id;
    strcpy(graph_asset.name, name);
    graph_asset.unique_node_id = unique_node_id;
    graph_asset.unique_pin_id = unique_pin_id;
    graph_asset.unique_link_id = unique_link_id;

    eg_vector_push(graph_assets, &graph_asset);
  }

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  return graph_assets;
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

  eg_string_appendf(sql, "INSERT INTO PIPELINE_ASSET (NAME, TYPE, LINK_INDEX, AUTO_CREATE_PIPELINE, AUTO_CREATE_VERTEX_INPUT_BUFFER, AUTO_LINK_DESCRIPTOR_BINDINGS, INTERLEAVED_VERTEX_INPUT_BUFFER, SOURCE, SPIRV)\n");
  eg_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)\n");
  eg_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  eg_string_appendf(sql, "TYPE = EXCLUDED.TYPE,\n");
  eg_string_appendf(sql, "LINK_INDEX = EXCLUDED.LINK_INDEX,\n");
  eg_string_appendf(sql, "AUTO_CREATE_PIPELINE = EXCLUDED.AUTO_CREATE_PIPELINE,\n");
  eg_string_appendf(sql, "AUTO_CREATE_VERTEX_INPUT_BUFFER = EXCLUDED.AUTO_CREATE_VERTEX_INPUT_BUFFER,\n");
  eg_string_appendf(sql, "AUTO_LINK_DESCRIPTOR_BINDINGS = EXCLUDED.AUTO_LINK_DESCRIPTOR_BINDINGS,\n");
  eg_string_appendf(sql, "INTERLEAVED_VERTEX_INPUT_BUFFER = EXCLUDED.INTERLEAVED_VERTEX_INPUT_BUFFER,\n");
  eg_string_appendf(sql, "SOURCE = EXCLUDED.SOURCE,\n");
  eg_string_appendf(sql, "SPIRV = EXCLUDED.SPIRV\n");

  sqlite3_stmt *stmt = 0;

  EG_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, eg_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_asset->type);
  sqlite3_bind_int(stmt, 3, pipeline_asset->link_index);
  sqlite3_bind_int(stmt, 4, pipeline_asset->auto_create_pipeline);
  sqlite3_bind_int(stmt, 5, pipeline_asset->auto_create_vertex_input_buffer);
  sqlite3_bind_int(stmt, 6, pipeline_asset->auto_link_descriptor_binding);
  sqlite3_bind_int(stmt, 7, pipeline_asset->interleaved_vertex_input_buffer);
  sqlite3_bind_text(stmt, 8, eg_string_buffer(pipeline_asset->source), -1, SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 9, pipeline_asset->spirv, (int32_t)pipeline_asset->spirv_size, SQLITE_STATIC);

  EG_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  eg_string_destroy(sql);

  if (pipeline_asset->id == 0) {
    pipeline_asset->id = eg_database_get_sequence_index_by_name("PIPELINE_ASSET");
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
  eg_string_destroy(pipeline_asset->source);

  if (pipeline_asset->spirv) {
    eg_heap_free(pipeline_asset->spirv);
  }
}
void eg_database_destroy_pipeline_assets(eg_vector_t *pipeline_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = eg_vector_count(pipeline_assets);

  while (asset_index < asset_count) {

    eg_pipeline_asset_t *asset = (eg_pipeline_asset_t *)eg_vector_at(pipeline_assets, asset_index);

    eg_string_destroy(asset->source);

    if (asset->spirv) {
      eg_heap_free(asset->spirv);
    }
  }

  eg_vector_destroy(pipeline_assets);
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

void eg_database_destroy_graph_asset(eg_graph_asset_t *graph_asset) {
  // TODO
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
    eg_string_appendf(sql, "AUTO_LINK_DESCRIPTOR_BINDING INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "INTERLEAVED_VERTEX_INPUT_BUFFER INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "SOURCE TEXT NOT NULL,\n");
    eg_string_appendf(sql, "SPIRV BLOB,\n");
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
    eg_string_appendf(sql, "UNIQUE_NODE_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "UNIQUE_PIN_ID INTEGER NOT NULL,\n");
    eg_string_appendf(sql, "UNIQUE_LINK_ID INTEGER NOT NULL,\n");
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
