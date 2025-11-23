#include <database/lb_pch.h>
#include <database/lb_database.h>

static int64_t lb_database_get_sequence_index_by_name(char const *table_name);

static sqlite3 *s_database_handle = 0;

void lb_database_create(void) {
  LB_SQL_CHECK(sqlite3_open(LB_FILE_PATH, &s_database_handle));
}
void lb_database_destroy(void) {
  LB_SQL_CHECK(sqlite3_close(s_database_handle));
}

lb_scene_asset_id_t lb_database_load_default_scene_asset_id(void) {
  lb_scene_asset_id_t scene_asset_id = -1;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT SCENE_ASSET.ID\n");
  lb_string_appendf(sql, "FROM SCENE_ASSET\n");
  lb_string_appendf(sql, "WHERE SCENE_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    scene_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return scene_asset_id;
}
lb_swapchain_asset_id_t lb_database_load_default_swapchain_asset_id(void) {
  lb_swapchain_asset_id_t swapchain_asset_id = -1;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID\n");
  lb_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");
  lb_string_appendf(sql, "WHERE SWAPCHAIN_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    swapchain_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return swapchain_asset_id;
}
lb_renderer_asset_id_t lb_database_load_default_renderer_asset_id(void) {
  lb_renderer_asset_id_t renderer_asset_id = -1;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT RENDERER_ASSET.ID\n");
  lb_string_appendf(sql, "FROM RENDERER_ASSET\n");
  lb_string_appendf(sql, "WHERE RENDERER_ASSET.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return renderer_asset_id;
}

lb_vector_t *lb_database_load_all_swapchain_assets(void) {
  lb_vector_t *swapchain_assets = lb_vector_create(sizeof(lb_swapchain_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID, SWAPCHAIN_ASSET.NAME, SWAPCHAIN_ASSET.IMAGE_COUNT, SWAPCHAIN_ASSET.IS_DEFAULT\n");
  lb_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_swapchain_asset_t swapchain_asset = {0};

    lb_swapchain_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    swapchain_asset.id = id;
    strcpy(swapchain_asset.name, name);
    swapchain_asset.image_count = image_count;
    swapchain_asset.is_default = is_default;

    lb_vector_push(swapchain_assets, &swapchain_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return swapchain_assets;
}
lb_swapchain_asset_t lb_database_load_swapchain_asset_by_id(lb_swapchain_asset_id_t swapchain_asset_id) {
  lb_swapchain_asset_t swapchain_asset = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT SWAPCHAIN_ASSET.ID, SWAPCHAIN_ASSET.NAME, SWAPCHAIN_ASSET.IMAGE_COUNT, SWAPCHAIN_ASSET.IS_DEFAULT\n");
  lb_string_appendf(sql, "FROM SWAPCHAIN_ASSET\n");
  lb_string_appendf(sql, "WHERE SWAPCHAIN_ASSET.ID = %lld\n", swapchain_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_swapchain_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);

    swapchain_asset.id = id;
    strcpy(swapchain_asset.name, name);
    swapchain_asset.image_count = image_count;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return swapchain_asset;
}

lb_vector_t *lb_database_load_all_renderer_assets(void) {
  lb_vector_t *renderer_assets = lb_vector_create(sizeof(lb_renderer_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT RENDERER_ASSET.ID, RENDERER_ASSET.NAME, RENDERER_ASSET.FRAMES_IN_FLIGHT, RENDERER_ASSET.IS_DEFAULT\n");
  lb_string_appendf(sql, "FROM RENDERER_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_asset_t renderer_asset = {0};

    lb_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    renderer_asset.id = id;
    strcpy(renderer_asset.name, name);
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;

    lb_vector_push(renderer_assets, &renderer_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return renderer_assets;
}
lb_renderer_asset_t lb_database_load_renderer_asset_by_id(lb_renderer_asset_id_t renderer_asset_id) {
  lb_renderer_asset_t renderer_asset = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT RENDERER_ASSET.ID, RENDERER_ASSET.NAME, RENDERER_ASSET.FRAMES_IN_FLIGHT, RENDERER_ASSET.IS_DEFAULT\n");
  lb_string_appendf(sql, "FROM RENDERER_ASSET\n");
  lb_string_appendf(sql, "WHERE RENDERER_ASSET.ID = %lld\n", renderer_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    renderer_asset.id = id;
    strcpy(renderer_asset.name, name);
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return renderer_asset;
}

uint64_t lb_database_load_vertex_input_binding_count_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_vertex_input_binding_count = 0;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT COUNT(*) AS CNT\n");
  lb_string_appendf(sql, "FROM PIPELINE_VERTEX_INPUT_BINDING\n");
  lb_string_appendf(sql, "WHERE PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_vertex_input_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_vertex_input_binding_count;
}
uint64_t lb_database_load_descriptor_binding_count_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  uint64_t pipeline_descriptor_binding_count = 0;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT COUNT(*) AS CNT\n");
  lb_string_appendf(sql, "FROM PIPELINE_DESCRIPTOR_BINDING\n");
  lb_string_appendf(sql, "WHERE PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ASSET_ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_descriptor_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_descriptor_binding_count;
}

lb_vector_t *lb_database_load_all_pipeline_assets(void) {
  lb_vector_t *pipeline_assets = lb_vector_create(sizeof(lb_pipeline_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_asset_t pipeline_asset = {0};

    lb_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    lb_pipeline_type_t type = (lb_pipeline_type_t)sqlite3_column_int(stmt, 2);
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

    lb_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_assets;
}
lb_vector_t *lb_database_load_all_pipeline_assets_by_type(lb_pipeline_type_t pipeline_type) {
  lb_vector_t *pipeline_assets = lb_vector_create(sizeof(lb_pipeline_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  lb_string_appendf(sql, "WHERE TYPE = %u\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_asset_t pipeline_asset = {0};

    lb_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    lb_pipeline_type_t type = (lb_pipeline_type_t)sqlite3_column_int(stmt, 2);
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

    lb_vector_push(pipeline_assets, &pipeline_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_assets;
}
lb_pipeline_asset_t lb_database_load_pipeline_asset_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  lb_pipeline_asset_t pipeline_asset = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_ASSET.ID, PIPELINE_ASSET.NAME, PIPELINE_ASSET.TYPE, PIPELINE_ASSET.LINK_INDEX, PIPELINE_ASSET.AUTO_CREATE_PIPELINE, PIPELINE_ASSET.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE_ASSET.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE_ASSET.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    lb_pipeline_type_t type = (lb_pipeline_type_t)sqlite3_column_int(stmt, 2);
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

  lb_string_destroy(sql);

  return pipeline_asset;
}
lb_pipeline_resource_t lb_database_load_pipeline_resource_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  lb_pipeline_resource_t pipeline_resource = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_RESOURCE.ID, PIPELINE_RESOURCE.VERTEX_SHADER_FILE_PATH, PIPELINE_RESOURCE.FRAGMENT_SHADER_FILE_PATH, PIPELINE_RESOURCE.COMPUTE_SHADER_FILE_PATH\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  lb_string_appendf(sql, "LEFT JOIN PIPELINE_RESOURCE\n");
  lb_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_RESOURCE.PIPELINE_ASSET_ID\n");
  lb_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *vertex_shader_file_path = sqlite3_column_text(stmt, 1);
    char const *fragment_shader_file_path = sqlite3_column_text(stmt, 2);
    char const *compute_shader_file_path = sqlite3_column_text(stmt, 3);

    uint64_t vertex_shader_file_path_size = vertex_shader_file_path ? strlen(vertex_shader_file_path) + 1 : 0;
    uint64_t fragment_shader_file_path_size = fragment_shader_file_path ? strlen(fragment_shader_file_path) + 1 : 0;
    uint64_t compute_shader_file_path_size = compute_shader_file_path ? strlen(compute_shader_file_path) + 1 : 0;

    pipeline_resource.id = id;
    pipeline_resource.pipeline_asset_id = pipeline_asset_id;
    strcpy(pipeline_resource.vertex_shader_file_path, vertex_shader_file_path);
    strcpy(pipeline_resource.fragment_shader_file_path, fragment_shader_file_path);
    strcpy(pipeline_resource.compute_shader_file_path, compute_shader_file_path);
    pipeline_resource.vertex_shader_file_path_size = vertex_shader_file_path_size;
    pipeline_resource.fragment_shader_file_path_size = fragment_shader_file_path_size;
    pipeline_resource.compute_shader_file_path_size = compute_shader_file_path_size;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_resource;
}
lb_vector_t *lb_database_load_all_pipeline_vertex_input_bindings_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  lb_vector_t *pipeline_vertex_input_bindings = lb_vector_create(sizeof(lb_pipeline_vertex_input_binding_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_VERTEX_INPUT_BINDING.ID, PIPELINE_VERTEX_INPUT_BINDING.NAME, PIPELINE_VERTEX_INPUT_BINDING.LOCATION, PIPELINE_VERTEX_INPUT_BINDING.SIZE, PIPELINE_VERTEX_INPUT_BINDING.COMPONENT_COUNT, PIPELINE_VERTEX_INPUT_BINDING.FORMAT, PIPELINE_VERTEX_INPUT_BINDING.INPUT_RATE\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  lb_string_appendf(sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDING\n");
  lb_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ASSET_ID\n");
  lb_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    lb_pipeline_vertex_input_binding_id_t id = sqlite3_column_int64(stmt, 0);
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

    lb_vector_push(pipeline_vertex_input_bindings, &pipeline_vertex_input_binding);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_vertex_input_bindings;
}
lb_vector_t *lb_database_load_all_pipeline_descriptor_bindings_by_id(lb_pipeline_asset_id_t pipeline_asset_id) {
  lb_vector_t *pipeline_descriptor_bindings = lb_vector_create(sizeof(lb_pipeline_descriptor_binding_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT PIPELINE_DESCRIPTOR_BINDING.ID, PIPELINE_DESCRIPTOR_BINDING.NAME, PIPELINE_DESCRIPTOR_BINDING.BINDING, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_TYPE, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_COUNT, PIPELINE_DESCRIPTOR_BINDING.STAGE_FLAGS, PIPELINE_DESCRIPTOR_BINDING.AUTO_BUFFER\n");
  lb_string_appendf(sql, "FROM PIPELINE_ASSET\n");
  lb_string_appendf(sql, "LEFT JOIN PIPELINE_DESCRIPTOR_BINDING\n");
  lb_string_appendf(sql, "ON PIPELINE_ASSET.ID = PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ASSET_ID\n");
  lb_string_appendf(sql, "WHERE PIPELINE_ASSET.ID = %lld\n", pipeline_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_descriptor_binding_t pipeline_descriptor_binding = {0};

    lb_pipeline_descriptor_binding_id_t id = sqlite3_column_int64(stmt, 0);
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

    lb_vector_push(pipeline_descriptor_bindings, &pipeline_descriptor_binding);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return pipeline_descriptor_bindings;
}

lb_vector_t *lb_database_load_all_models(void) {
  lb_vector_t *model_assets = lb_vector_create(sizeof(lb_model_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT MODEL_ASSET.ID, MODEL_ASSET.NAME\n");
  lb_string_appendf(sql, "FROM MODEL_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_asset_t model_asset = {0};

    lb_model_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    model_asset.id = id;
    strcpy(model_asset.name, name);

    lb_vector_push(model_assets, &model_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return model_assets;
}
lb_model_resource_t lb_database_load_model_resource_by_id(lb_model_asset_id_t model_asset_id) {
  lb_model_resource_t model_resource = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT MODEL_RESOURCE.ID, MODEL_RESOURCE.MODEL_FILE_PATH\n");
  lb_string_appendf(sql, "FROM MODEL_ASSET\n");
  lb_string_appendf(sql, "LEFT JOIN MODEL_RESOURCE\n");
  lb_string_appendf(sql, "ON MODEL_ASSET.ID = MODEL_RESOURCE.MODEL_ASSET_ID\n");
  lb_string_appendf(sql, "WHERE MODEL_ASSET.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *model_file_path = sqlite3_column_text(stmt, 1);

    model_resource.id = id;
    model_resource.model_asset_id = model_asset_id;
    strcpy(model_resource.model_file_path, model_file_path);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return model_resource;
}
lb_vector_t *lb_database_load_all_model_meshes_by_id(lb_model_asset_id_t model_asset_id) {
  lb_vector_t *model_meshes = lb_vector_create(sizeof(lb_model_mesh_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT MODEL_MESH.ID, MODEL_MESH.NAME\n");
  lb_string_appendf(sql, "FROM MODEL_ASSET\n");
  lb_string_appendf(sql, "LEFT JOIN MODEL_MESH\n");
  lb_string_appendf(sql, "ON MODEL_ASSET.ID = MODEL_MESH.MODEL_ASSET_ID\n");
  lb_string_appendf(sql, "WHERE MODEL_ASSET.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_mesh_t model_mesh = {0};

    lb_model_mesh_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    model_mesh.id = id;
    model_mesh.model_asset_id = model_asset_id;
    strcpy(model_mesh.name, name);

    lb_vector_push(model_meshes, &model_mesh);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return model_meshes;
}

lb_vector_t *lb_database_load_all_mesh_primitives_by_id(lb_model_mesh_id_t model_mesh_id) {
  lb_vector_t *mesh_primitives = lb_vector_create(sizeof(lb_mesh_primitive_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT MESH_PRIMITIVE.ID, MESH_PRIMITIVE.NAME\n");
  lb_string_appendf(sql, "FROM MODEL_MESH\n");
  lb_string_appendf(sql, "LEFT JOIN MESH_PRIMITIVE\n");
  lb_string_appendf(sql, "ON MODEL_MESH.ID = MESH_PRIMITIVE.MODEL_MESH_ID\n");
  lb_string_appendf(sql, "WHERE MODEL_MESH.ID = %lld\n", model_mesh_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_mesh_primitive_t mesh_primitive = {0};

    lb_mesh_primitive_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    mesh_primitive.id = id;
    mesh_primitive.model_mesh_id = model_mesh_id;
    strcpy(mesh_primitive.name, name);

    lb_vector_push(mesh_primitives, &mesh_primitive);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return mesh_primitives;
}
lb_vector_t *lb_database_load_all_mesh_attributes_by_id(lb_mesh_primitive_id_t mesh_primitive_id) {
  lb_vector_t *mesh_attributes = lb_vector_create(sizeof(lb_mesh_attribute_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT MESH_ATTRIBUTE.ID, MESH_ATTRIBUTE.NAME, MESH_ATTRIBUTE.TYPE, MESH_ATTRIBUTE.COUNT\n");
  lb_string_appendf(sql, "FROM MESH_PRIMITIVE\n");
  lb_string_appendf(sql, "LEFT JOIN MESH_ATTRIBUTE\n");
  lb_string_appendf(sql, "ON MESH_PRIMITIVE.ID = MESH_ATTRIBUTE.MESH_PRIMITIVE_ID\n");
  lb_string_appendf(sql, "WHERE MESH_PRIMITIVE.ID = %lld\n", mesh_primitive_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_mesh_attribute_t mesh_attribute = {0};

    lb_mesh_attribute_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = sqlite3_column_int(stmt, 2);
    uint64_t count = sqlite3_column_int64(stmt, 3);

    mesh_attribute.id = id;
    mesh_attribute.mesh_primitive_id = mesh_primitive_id;
    strcpy(mesh_attribute.name, name);
    mesh_attribute.type = type;
    mesh_attribute.count = count;

    lb_vector_push(mesh_attributes, &mesh_attribute);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return mesh_attributes;
}

lb_attribute_buffer_t lb_database_load_attribute_buffer_by_id(lb_mesh_attribute_id_t mesh_attribute_id) {
  lb_attribute_buffer_t attribute_buffer = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT ATTRIBUTE_BUFFER.ID, ATTRIBUTE_BUFFER.DATA\n");
  lb_string_appendf(sql, "FROM MESH_ATTRIBUTE\n");
  lb_string_appendf(sql, "LEFT JOIN ATTRIBUTE_BUFFER\n");
  lb_string_appendf(sql, "ON MESH_ATTRIBUTE.ID = ATTRIBUTE_BUFFER.MESH_ATTRIBUTE_ID\n");
  lb_string_appendf(sql, "WHERE MESH_ATTRIBUTE.ID = %lld\n", mesh_attribute_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_attribute_data_id_t id = sqlite3_column_int64(stmt, 0);
    uint8_t const *data = sqlite3_column_blob(stmt, 1);
    uint64_t data_size = sqlite3_column_bytes(stmt, 1);

    attribute_buffer.id = id;
    attribute_buffer.mesh_attribute_id = mesh_attribute_id;
    attribute_buffer.data = lb_heap_alloc(data_size, 0, data);
    attribute_buffer.data_size = data_size;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return attribute_buffer;
}

lb_vector_t *lb_database_load_all_scene_assets(void) {
  lb_vector_t *scene_assets = lb_vector_create(sizeof(lb_scene_asset_t));

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT SCENE_ASSET.ID, SCENE_ASSET.NAME\n");
  lb_string_appendf(sql, "FROM SCENE_ASSET\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_scene_asset_t scene_asset = {0};

    lb_scene_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    scene_asset.id = id;
    strcpy(scene_asset.name, name);

    lb_vector_push(scene_assets, &scene_asset);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return scene_assets;
}

lb_graph_asset_t lb_database_load_graph_asset_by_id(lb_graph_asset_id_t graph_asset_id) {
  lb_graph_asset_t graph_asset = {0};

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT GRAPH_ASSET.ID, GRAPH_ASSET.NAME\n");
  lb_string_appendf(sql, "FROM GRAPH_ASSET\n");
  lb_string_appendf(sql, "WHERE GRAPH_ASSET.ID = %lld\n", graph_asset_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    graph_asset.id = id;
    strcpy(graph_asset.name, name);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return graph_asset;
}

void lb_database_store_swapchain_asset(lb_swapchain_asset_t *swapchain_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO SWAPCHAIN_ASSET (NAME, IMAGE_COUNT)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  lb_string_appendf(sql, "IMAGE_COUNT = EXCLUDED.IMAGE_COUNT,\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, swapchain_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, swapchain_asset->image_count);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (swapchain_asset->id == 0) {
    swapchain_asset->id = lb_database_get_sequence_index_by_name("SWAPCHAIN_ASSET");
  }
}

void lb_database_store_renderer_asset(lb_renderer_asset_t *renderer_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO RENDERER_ASSET (NAME, FRAMES_IN_FLIGHT)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  lb_string_appendf(sql, "FRAMES_IN_FLIGHT = EXCLUDED.FRAMES_IN_FLIGHT\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, renderer_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, renderer_asset->frames_in_flight);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (renderer_asset->id == 0) {
    renderer_asset->id = lb_database_get_sequence_index_by_name("RENDERER_ASSET");
  }
}

void lb_database_store_pipeline_asset(lb_pipeline_asset_t *pipeline_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO PIPELINE_ASSET (NAME, TYPE, LINK_INDEX, AUTO_CREATE_PIPELINE, AUTO_CREATE_VERTEX_INPUT_BUFFER, AUTO_LINK_DESCRIPTOR_BINDINGS, INTERLEAVED_VERTEX_INPUT_BUFFER)\n");
  lb_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  lb_string_appendf(sql, "TYPE = EXCLUDED.TYPE,\n");
  lb_string_appendf(sql, "LINK_INDEX = EXCLUDED.LINK_INDEX,\n");
  lb_string_appendf(sql, "AUTO_CREATE_PIPELINE = EXCLUDED.AUTO_CREATE_PIPELINE,\n");
  lb_string_appendf(sql, "AUTO_CREATE_VERTEX_INPUT_BUFFER = EXCLUDED.AUTO_CREATE_VERTEX_INPUT_BUFFER,\n");
  lb_string_appendf(sql, "AUTO_LINK_DESCRIPTOR_BINDINGS = EXCLUDED.AUTO_LINK_DESCRIPTOR_BINDINGS,\n");
  lb_string_appendf(sql, "INTERLEAVED_VERTEX_INPUT_BUFFER = EXCLUDED.INTERLEAVED_VERTEX_INPUT_BUFFER\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, pipeline_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, pipeline_asset->type);
  sqlite3_bind_int(stmt, 3, pipeline_asset->link_index);
  sqlite3_bind_int(stmt, 4, pipeline_asset->auto_create_pipeline);
  sqlite3_bind_int(stmt, 5, pipeline_asset->auto_create_vertex_input_buffer);
  sqlite3_bind_int(stmt, 6, pipeline_asset->auto_link_descriptor_bindings);
  sqlite3_bind_int(stmt, 7, pipeline_asset->interleaved_vertex_input_buffer);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (pipeline_asset->id == 0) {
    pipeline_asset->id = lb_database_get_sequence_index_by_name("PIPELINE_ASSET");
  }
}
void lb_database_store_pipeline_resource(lb_pipeline_resource_t *pipeline_resource) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO PIPELINE_RESOURCE (PIPELINE_ASSET_ID, VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH, COMPUTE_SHADER_FILE_PATH)\n");
  lb_string_appendf(sql, "VALUES (?, ?, ?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (PIPELINE_ID) DO UPDATE SET\n");
  lb_string_appendf(sql, "VERTEX_SHADER_FILE_PATH = EXCLUDED.VERTEX_SHADER_FILE_PATH,\n");
  lb_string_appendf(sql, "FRAGMENT_SHADER_FILE_PATH = EXCLUDED.FRAGMENT_SHADER_FILE_PATH,\n");
  lb_string_appendf(sql, "COMPUTE_SHADER_FILE_PATH = EXCLUDED.COMPUTE_SHADER_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_resource->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_resource->vertex_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pipeline_resource->fragment_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pipeline_resource->compute_shader_file_path, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (pipeline_resource->id == 0) {
    pipeline_resource->id = lb_database_get_sequence_index_by_name("PIPELINE_RESOURCE");
  }
}
void lb_database_store_pipeline_vertex_input_binding(lb_pipeline_vertex_input_binding_t *pipeline_vertex_input_binding) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDING (PIPELINE_ASSET_ID, NAME, LOCATION, SIZE, COMPONENT_COUNT, FORMAT, INPUT_RATE)\n");
  lb_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (PIPELINE_ID, LOCATION) DO UPDATE SET\n");
  lb_string_appendf(sql, "INPUT_RATE = EXCLUDED.INPUT_RATE\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_vertex_input_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_vertex_input_binding->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_vertex_input_binding->location);
  sqlite3_bind_int(stmt, 4, pipeline_vertex_input_binding->size);
  sqlite3_bind_int(stmt, 5, pipeline_vertex_input_binding->component_count);
  sqlite3_bind_int(stmt, 6, pipeline_vertex_input_binding->format);
  sqlite3_bind_int(stmt, 7, pipeline_vertex_input_binding->input_rate);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (pipeline_vertex_input_binding->id == 0) {
    pipeline_vertex_input_binding->id = lb_database_get_sequence_index_by_name("PIPELINE_VERTEX_INPUT_BINDING");
  }
}
void lb_database_store_pipeline_descriptor_binding(lb_pipeline_descriptor_binding_t *pipeline_descriptor_binding) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO PIPELINE_DESCRIPTOR_BINDING (PIPELINE_ASSET_ID, NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  lb_string_appendf(sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (PIPELINE_ID, BINDING) DO UPDATE SET\n");
  lb_string_appendf(sql, "STAGE_FLAGS = EXCLUDED.STAGE_FLAGS | PIPELINE_DESCRIPTOR_BINDINGS.STAGE_FLAGS,\n");
  lb_string_appendf(sql, "AUTO_BUFFER = EXCLUDED.AUTO_BUFFER\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_descriptor_binding->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_descriptor_binding->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, pipeline_descriptor_binding->binding);
  sqlite3_bind_int(stmt, 4, pipeline_descriptor_binding->descriptor_type);
  sqlite3_bind_int(stmt, 5, pipeline_descriptor_binding->descriptor_count);
  sqlite3_bind_int(stmt, 6, pipeline_descriptor_binding->stage_flags);
  sqlite3_bind_int(stmt, 7, pipeline_descriptor_binding->auto_buffer);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (pipeline_descriptor_binding->id == 0) {
    pipeline_descriptor_binding->id = lb_database_get_sequence_index_by_name("PIPELINE_DESCRIPTOR_BINDING");
  }
}

void lb_database_store_model_asset(lb_model_asset_t *model_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO MODEL_ASSET (NAME)\n");
  lb_string_appendf(sql, "VALUES (?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, model_asset->name, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (model_asset->id == 0) {
    model_asset->id = lb_database_get_sequence_index_by_name("MODEL_ASSET");
  }
}
void lb_database_store_model_resource(lb_model_resource_t *model_resource) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO MODEL_RESOURCE (MODEL_ASSET_ID, MODEL_FILE_PATH)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (MODEL_ID) DO UPDATE SET\n");
  lb_string_appendf(sql, "MODEL_FILE_PATH = EXCLUDED.MODEL_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_resource->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_resource->model_file_path, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (model_resource->id == 0) {
    model_resource->id = lb_database_get_sequence_index_by_name("MODEL_RESOURCE");
  }
}
void lb_database_store_model_mesh(lb_model_mesh_t *model_mesh) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO MODEL_MESH (MODEL_ASSET_ID, NAME)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");
  lb_string_appendf(sql, "ON CONFLICT (MODEL_ID) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_mesh->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_mesh->name, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (model_mesh->id == 0) {
    model_mesh->id = lb_database_get_sequence_index_by_name("MODEL_MESH");
  }
}

void lb_database_store_mesh_primitive(lb_mesh_primitive_t *mesh_primitive) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO MESH_PRIMITIVE (MODEL_MESH_ID, NAME)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_primitive->model_mesh_id);
  sqlite3_bind_text(stmt, 2, mesh_primitive->name, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (mesh_primitive->id == 0) {
    mesh_primitive->id = lb_database_get_sequence_index_by_name("MESH_PRIMITIVE");
  }
}
void lb_database_store_mesh_attribute(lb_mesh_attribute_t *mesh_attribute) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO MESH_ATTRIBUTE (MESH_PRIMITIVE_ID, NAME, TYPE, COUNT)\n");
  lb_string_appendf(sql, "VALUES (?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_attribute->mesh_primitive_id);
  sqlite3_bind_text(stmt, 2, mesh_attribute->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, mesh_attribute->type);
  sqlite3_bind_int64(stmt, 4, mesh_attribute->count);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (mesh_attribute->id == 0) {
    mesh_attribute->id = lb_database_get_sequence_index_by_name("MESH_ATTRIBUTE");
  }
}

void lb_database_store_attribute_buffer(lb_attribute_buffer_t *attribute_buffer) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO ATTRIBUTE_BUFFER (MESH_ATTRIBUTE_ID, DATA)\n");
  lb_string_appendf(sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, attribute_buffer->mesh_attribute_id);
  sqlite3_bind_blob(stmt, 2, attribute_buffer->data, (int32_t)attribute_buffer->data_size, SQLITE_STATIC);
  sqlite3_bind_int64(stmt, 3, attribute_buffer->data_size);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (attribute_buffer->id == 0) {
    attribute_buffer->id = lb_database_get_sequence_index_by_name("ATTRIBUTE_BUFFER");
  }
}

void lb_database_store_scene(lb_scene_asset_t *scene_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO SCENE_ASSET (NAME)\n");
  lb_string_appendf(sql, "VALUES (?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, scene_asset->name, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (scene_asset->id == 0) {
    scene_asset->id = lb_database_get_sequence_index_by_name("SCENE_ASSET");
  }
}

void lb_database_store_graph_asset(lb_graph_asset_t *graph_asset) {
  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "INSERT INTO GRAPH_ASSET (NAME)\n");
  lb_string_appendf(sql, "VALUES (?)\n");
  lb_string_appendf(sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, graph_asset->name, -1, SQLITE_STATIC);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  if (graph_asset->id == 0) {
    graph_asset->id = lb_database_get_sequence_index_by_name("GRAPH_ASSET");
  }
}

void lb_database_destroy_swapchain(lb_swapchain_asset_t *swapchain_asset) {
  // TODO
}
void lb_database_destroy_swapchain_assets(lb_vector_t *swapchain_assets) {
  // TODO

  lb_vector_destroy(swapchain_assets);
}

void lb_database_destroy_renderer_asset(lb_renderer_asset_t *renderer_asset) {
  // TODO
}
void lb_database_destroy_renderer_assets(lb_vector_t *renderer_assets) {
  // TODO

  lb_vector_destroy(renderer_assets);
}

void lb_database_destroy_pipeline_asset(lb_pipeline_asset_t *pipeline_asset) {
  // TODO
}
void lb_database_destroy_pipeline_assets(lb_vector_t *pipeline_assets) {
  // TODO

  lb_vector_destroy(pipeline_assets);
}
void lb_database_destroy_pipeline_resource(lb_pipeline_resource_t *pipeline_resource) {
  // TODO
}
void lb_database_destroy_pipeline_vertex_input_bindings(lb_vector_t *pipeline_vertex_input_bindings) {
  // TODO

  lb_vector_destroy(pipeline_vertex_input_bindings);
}
void lb_database_destroy_pipeline_descriptor_bindings(lb_vector_t *pipeline_descriptor_bindings) {
  // TODO

  lb_vector_destroy(pipeline_descriptor_bindings);
}

void lb_database_destroy_model_assets(lb_vector_t *model_assets) {
  // TODO

  lb_vector_destroy(model_assets);
}
void lb_database_destroy_model_resource(lb_model_resource_t *model_resource) {
  // TODO
}
void lb_database_destroy_model_meshes(lb_vector_t *model_meshes) {
  // TODO

  lb_vector_destroy(model_meshes);
}

void lb_database_destroy_mesh_primitives(lb_vector_t *mesh_primitives) {
  // TODO

  lb_vector_destroy(mesh_primitives);
}
void lb_database_destroy_mesh_attributes(lb_vector_t *mesh_attributes) {
  // TODO

  lb_vector_destroy(mesh_attributes);
}

void lb_database_destroy_attribute_buffer(lb_attribute_buffer_t *attribute_buffer) {
  lb_heap_free(attribute_buffer->data);
}

void lb_database_destroy_scene_assets(lb_vector_t *scene_assets) {
  // TODO

  lb_vector_destroy(scene_assets);
}

void lb_database_destroy_graph_assets(lb_vector_t *graph_assets) {
  // TODO

  lb_vector_destroy(graph_assets);
}

static int64_t lb_database_get_sequence_index_by_name(char const *table_name) {
  int64_t sequence_id = 1;

  lb_string_t *sql = lb_string_create();

  lb_string_appendf(sql, "SELECT seq\n");
  lb_string_appendf(sql, "FROM sqlite_sequence\n");
  lb_string_appendf(sql, "WHERE name = '%s'\n", table_name);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    sequence_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(sql);

  return sequence_id;
}
