#include <library/lb_pch.h>

#include <library/database/lb_database.h>

#include <sqlite3/sqlite3.h>

static int64_t database_get_sequence_index_by_name(char const *table_name);

static sqlite3 *s_database_handle = 0;

void database_create(void) {
  SQL_CHECK(sqlite3_open(DB_FILE_PATH, &s_database_handle));
}
void database_destroy(void) {
  SQL_CHECK(sqlite3_close(s_database_handle));
}

scene_asset_id_t database_load_default_scene_asset_id(void) {
  scene_asset_id_t scene_asset_id = -1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT SA.ID\n");
  string_appendf(&sql, "FROM SCENE_ASSETS AS SA\n");
  string_appendf(&sql, "WHERE SA.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    scene_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return scene_asset_id;
}
swapchain_asset_id_t database_load_default_swapchain_asset_id(void) {
  swapchain_asset_id_t swapchain_asset_id = -1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT SA.ID\n");
  string_appendf(&sql, "FROM SWAPCHAIN_ASSETS AS SA\n");
  string_appendf(&sql, "WHERE SA.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    swapchain_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return swapchain_asset_id;
}
renderer_asset_id_t database_load_default_renderer_asset_id(void) {
  renderer_asset_id_t renderer_asset_id = -1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT RA.ID\n");
  string_appendf(&sql, "FROM RENDERER_ASSETS AS RA\n");
  string_appendf(&sql, "WHERE RA.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return renderer_asset_id;
}

vector_t database_load_swapchain_assets(void) {
  vector_t swapchain_assets = vector_create(sizeof(swapchain_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT SA.ID, SA.NAME, SA.IMAGE_COUNT, SA.DEPTH_FORMAT, SA.IS_DEFAULT\n");
  string_appendf(&sql, "FROM SWAPCHAIN_ASSETS AS SA\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    swapchain_asset_t swapchain_asset = {0};

    pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    int32_t depth_format = sqlite3_column_int(stmt, 3);
    uint8_t is_default = sqlite3_column_int(stmt, 4);

    uint64_t name_size = strlen(name) + 1;

    swapchain_asset.id = id;
    swapchain_asset.name = heap_alloc(name_size, 1, name);
    swapchain_asset.name_size = name_size;
    swapchain_asset.image_count = image_count;
    swapchain_asset.depth_format = depth_format;
    swapchain_asset.is_default = is_default;

    vector_push(&swapchain_assets, &swapchain_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return swapchain_assets;
}
swapchain_asset_t database_load_swapchain_asset_by_id(swapchain_asset_id_t swapchain_asset_id) {
  swapchain_asset_t swapchain_asset = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT SA.ID, SA.NAME, SA.IMAGE_COUNT, SA.DEPTH_FORMAT, SA.IS_DEFAULT\n");
  string_appendf(&sql, "FROM SWAPCHAIN_ASSETS AS SA\n");
  string_appendf(&sql, "WHERE SA.ID = %lld\n", swapchain_asset_id);

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

vector_t database_load_renderer_assets(void) {
  vector_t renderer_assets = vector_create(sizeof(renderer_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT RA.ID, RA.NAME, RA.FRAMES_IN_FLIGHT, RA.IS_DEFAULT\n");
  string_appendf(&sql, "FROM RENDERER_ASSETS AS RA\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_t renderer_asset = {0};

    pipeline_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    renderer_asset.id = id;
    renderer_asset.name = heap_alloc(name_size, 1, name);
    renderer_asset.name_size = name_size;
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;

    vector_push(&renderer_assets, &renderer_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return renderer_assets;
}
renderer_asset_t database_load_renderer_asset_by_id(renderer_asset_id_t renderer_asset_id) {
  renderer_asset_t renderer_asset = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT RA.ID, RA.NAME, RA.FRAMES_IN_FLIGHT, RA.IS_DEFAULT\n");
  string_appendf(&sql, "FROM RENDERER_ASSETS AS RA\n");
  string_appendf(&sql, "WHERE RA.ID = %lld\n", renderer_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    renderer_asset.id = id;
    renderer_asset.name = heap_alloc(name_size, 1, name);
    renderer_asset.name_size = name_size;
    renderer_asset.frames_in_flight = frames_in_flight;
    renderer_asset.is_default = is_default;
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

  string_appendf(&sql, "SELECT PA.ID, PA.NAME, PA.TYPE, PA.LINK_INDEX, PA.AUTO_CREATE_PIPELINE, PA.AUTO_CREATE_VERTEX_INPUT_BUFFER, PA.AUTO_LINK_DESCRIPTOR_BINDINGS, PA.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");

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

  string_appendf(&sql, "SELECT PA.ID, PA.NAME, PA.TYPE, PA.LINK_INDEX, PA.AUTO_CREATE_PIPELINE, PA.AUTO_CREATE_VERTEX_INPUT_BUFFER, PA.AUTO_LINK_DESCRIPTOR_BINDINGS, PA.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  string_appendf(&sql, "FROM PIPELINE_ASSETS AS PA\n");
  string_appendf(&sql, "WHERE TYPE = %u\n", pipeline_type);

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
model_resource_t database_load_model_resource_by_id(model_asset_id_t model_asset_id) {
  model_resource_t model_resource = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT MR.ID, MR.MODEL_FILE_PATH\n");
  string_appendf(&sql, "FROM MODEL_ASSETS AS MA\n");
  string_appendf(&sql, "LEFT JOIN MODEL_RESOURCES AS MR\n");
  string_appendf(&sql, "ON MA.ID = MR.MODEL_ASSET_ID\n");
  string_appendf(&sql, "WHERE MA.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    model_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *model_file_path = sqlite3_column_text(stmt, 1);

    uint64_t model_file_path_size = model_file_path ? strlen(model_file_path) + 1 : 0;

    model_resource.id = id;
    model_resource.model_asset_id = model_asset_id;
    model_resource.model_file_path = model_file_path ? heap_alloc(model_file_path_size, 1, model_file_path) : 0;
    model_resource.model_file_path_size = model_file_path_size;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return model_resource;
}
vector_t database_load_model_meshes_by_id(model_asset_id_t model_asset_id) {
  vector_t model_meshes = vector_create(sizeof(model_mesh_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT MM.ID, MM.NAME\n");
  string_appendf(&sql, "FROM MODEL_ASSETS AS MA\n");
  string_appendf(&sql, "LEFT JOIN MODEL_MESHES AS MM\n");
  string_appendf(&sql, "ON MA.ID = MM.MODEL_ASSET_ID\n");
  string_appendf(&sql, "WHERE MA.ID = %lld\n", model_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    model_mesh_t model_mesh = {0};

    model_mesh_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    model_mesh.id = id;
    model_mesh.model_asset_id = model_asset_id;
    model_mesh.name = heap_alloc(name_size, 1, name);
    model_mesh.name_size = name_size;

    vector_push(&model_meshes, &model_mesh);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return model_meshes;
}

vector_t database_load_mesh_primitives_by_id(model_mesh_id_t model_mesh_id) {
  vector_t mesh_primitives = vector_create(sizeof(mesh_primitive_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT MP.ID, MP.NAME\n");
  string_appendf(&sql, "FROM MODEL_MESHES AS MM\n");
  string_appendf(&sql, "LEFT JOIN MESH_PRIMITIVES AS MP\n");
  string_appendf(&sql, "ON MM.ID = MP.MODEL_MESH_ID\n");
  string_appendf(&sql, "WHERE MM.ID = %lld\n", model_mesh_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    mesh_primitive_t mesh_primitive = {0};

    mesh_primitive_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    mesh_primitive.id = id;
    mesh_primitive.model_mesh_id = model_mesh_id;
    mesh_primitive.name = heap_alloc(name_size, 1, name);
    mesh_primitive.name_size = name_size;

    vector_push(&mesh_primitives, &mesh_primitive);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return mesh_primitives;
}
vector_t database_load_mesh_attributes_by_id(mesh_primitive_id_t mesh_primitive_id) {
  vector_t mesh_attributes = vector_create(sizeof(mesh_attribute_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT MA.ID, MA.NAME, MA.TYPE, MA.COUNT\n");
  string_appendf(&sql, "FROM MESH_PRIMITIVES AS MP\n");
  string_appendf(&sql, "LEFT JOIN MESH_ATTRIBUTES AS MA\n");
  string_appendf(&sql, "ON MP.ID = MA.MESH_PRIMITIVE_ID\n");
  string_appendf(&sql, "WHERE MP.ID = %lld\n", mesh_primitive_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    mesh_attribute_t mesh_attribute = {0};

    mesh_attribute_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = sqlite3_column_int(stmt, 2);
    uint64_t count = sqlite3_column_int64(stmt, 3);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    mesh_attribute.id = id;
    mesh_attribute.mesh_primitive_id = mesh_primitive_id;
    mesh_attribute.name = heap_alloc(name_size, 1, name);
    mesh_attribute.name_size = name_size;
    mesh_attribute.type = type;
    mesh_attribute.count = count;

    vector_push(&mesh_attributes, &mesh_attribute);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return mesh_attributes;
}

attribute_buffer_t database_load_attribute_buffer_by_id(mesh_attribute_id_t mesh_attribute_id) {
  attribute_buffer_t attribute_buffer = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT AB.ID, AB.DATA\n");
  string_appendf(&sql, "FROM MESH_ATTRIBUTES AS MA\n");
  string_appendf(&sql, "LEFT JOIN ATTRIBUTE_BUFFER AS AB\n");
  string_appendf(&sql, "ON MA.ID = AB.MESH_ATTRIBUTE_ID\n");
  string_appendf(&sql, "WHERE MA.ID = %lld\n", mesh_attribute_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    attribute_data_id_t id = sqlite3_column_int64(stmt, 0);
    uint8_t const *data = sqlite3_column_blob(stmt, 1);
    uint64_t data_size = sqlite3_column_bytes(stmt, 1);

    attribute_buffer.id = id;
    attribute_buffer.mesh_attribute_id = mesh_attribute_id;
    attribute_buffer.data = heap_alloc(data_size, 0, data);
    attribute_buffer.data_size = data_size;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return attribute_buffer;
}

vector_t database_load_scene_assets(void) {
  vector_t scene_assets = vector_create(sizeof(scene_asset_t));

  string_t sql = string_create();

  string_appendf(&sql, "SELECT ID, NAME FROM SCENE_ASSETS\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    scene_asset_t scene_asset = {0};

    scene_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    scene_asset.id = id;
    scene_asset.name = heap_alloc(name_size, 1, name);
    scene_asset.name_size = name_size;

    vector_push(&scene_assets, &scene_asset);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return scene_assets;
}

graph_asset_t database_load_graph_asset_by_id(graph_asset_id_t graph_asset_id) {
  graph_asset_t graph_asset = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT GA.ID, GA.NAME\n");
  string_appendf(&sql, "FROM GRAPH_ASSETS AS GA\n");
  string_appendf(&sql, "WHERE GA.ID = %lld\n", graph_asset_id);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_asset_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = strlen(name) + 1;

    graph_asset.id = id;
    graph_asset.name = heap_alloc(name_size, 1, name);
    graph_asset.name_size = name_size;
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return graph_asset;
}

void database_store_swapchain_asset(swapchain_asset_t *swapchain_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO SWAPCHAIN_ASSETS (NAME, IMAGE_COUNT, DEPTH_FORMAT)\n");
  string_appendf(&sql, "VALUES (?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  string_appendf(&sql, "IMAGE_COUNT = EXCLUDED.IMAGE_COUNT,\n");
  string_appendf(&sql, "DEPTH_FORMAT = EXCLUDED.DEPTH_FORMAT\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, swapchain_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, swapchain_asset->image_count);
  sqlite3_bind_int(stmt, 3, swapchain_asset->depth_format);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (swapchain_asset->id == 0) {
    swapchain_asset->id = database_get_sequence_index_by_name("SWAPCHAIN_ASSETS");
  }
}

void database_store_renderer_asset(renderer_asset_t *renderer_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO RENDERER_ASSETS (NAME, FRAMES_IN_FLIGHT)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  string_appendf(&sql, "FRAMES_IN_FLIGHT = EXCLUDED.FRAMES_IN_FLIGHT\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, renderer_asset->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, renderer_asset->frames_in_flight);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (renderer_asset->id == 0) {
    renderer_asset->id = database_get_sequence_index_by_name("RENDERER_ASSETS");
  }
}

void database_store_pipeline_asset(pipeline_asset_t *pipeline_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_ASSETS (NAME, TYPE, LINK_INDEX, AUTO_CREATE_PIPELINE, AUTO_CREATE_VERTEX_INPUT_BUFFER, AUTO_LINK_DESCRIPTOR_BINDINGS, INTERLEAVED_VERTEX_INPUT_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  string_appendf(&sql, "TYPE = EXCLUDED.TYPE,\n");
  string_appendf(&sql, "LINK_INDEX = EXCLUDED.LINK_INDEX,\n");
  string_appendf(&sql, "AUTO_CREATE_PIPELINE = EXCLUDED.AUTO_CREATE_PIPELINE,\n");
  string_appendf(&sql, "AUTO_CREATE_VERTEX_INPUT_BUFFER = EXCLUDED.AUTO_CREATE_VERTEX_INPUT_BUFFER,\n");
  string_appendf(&sql, "AUTO_LINK_DESCRIPTOR_BINDINGS = EXCLUDED.AUTO_LINK_DESCRIPTOR_BINDINGS,\n");
  string_appendf(&sql, "INTERLEAVED_VERTEX_INPUT_BUFFER = EXCLUDED.INTERLEAVED_VERTEX_INPUT_BUFFER\n");

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

  if (pipeline_asset->id == 0) {
    pipeline_asset->id = database_get_sequence_index_by_name("PIPELINE_ASSETS");
  }
}
void database_store_pipeline_resource(pipeline_resource_t *pipeline_resource) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_RESOURCES (PIPELINE_ASSET_ID, VERTEX_SHADER_FILE_PATH, FRAGMENT_SHADER_FILE_PATH, COMPUTE_SHADER_FILE_PATH)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID) DO UPDATE SET\n");
  string_appendf(&sql, "VERTEX_SHADER_FILE_PATH = EXCLUDED.VERTEX_SHADER_FILE_PATH,\n");
  string_appendf(&sql, "FRAGMENT_SHADER_FILE_PATH = EXCLUDED.FRAGMENT_SHADER_FILE_PATH,\n");
  string_appendf(&sql, "COMPUTE_SHADER_FILE_PATH = EXCLUDED.COMPUTE_SHADER_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, pipeline_resource->pipeline_asset_id);
  sqlite3_bind_text(stmt, 2, pipeline_resource->vertex_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, pipeline_resource->fragment_shader_file_path, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, pipeline_resource->compute_shader_file_path, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (pipeline_resource->id == 0) {
    pipeline_resource->id = database_get_sequence_index_by_name("PIPELINE_RESOURCES");
  }
}
void database_store_pipeline_vertex_input_binding(pipeline_vertex_input_binding_t *pipeline_vertex_input_binding) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_VERTEX_INPUT_BINDINGS (PIPELINE_ASSET_ID, BINDING_NAME, LOCATION, SIZE, COMPONENT_COUNT, FORMAT, INPUT_RATE)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID, LOCATION) DO UPDATE SET\n");
  string_appendf(&sql, "INPUT_RATE = EXCLUDED.INPUT_RATE\n");

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

  if (pipeline_vertex_input_binding->id == 0) {
    pipeline_vertex_input_binding->id = database_get_sequence_index_by_name("PIPELINE_VERTEX_INPUT_BINDINGS");
  }
}
void database_store_pipeline_descriptor_binding(pipeline_descriptor_binding_t *pipeline_descriptor_binding) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO PIPELINE_DESCRIPTOR_BINDINGS (PIPELINE_ASSET_ID, BINDING_NAME, BINDING, DESCRIPTOR_TYPE, DESCRIPTOR_COUNT, STAGE_FLAGS, AUTO_BUFFER)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?, ?, ?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (PIPELINE_ASSET_ID, BINDING) DO UPDATE SET\n");
  string_appendf(&sql, "STAGE_FLAGS = EXCLUDED.STAGE_FLAGS | PIPELINE_DESCRIPTOR_BINDINGS.STAGE_FLAGS,\n");
  string_appendf(&sql, "AUTO_BUFFER = EXCLUDED.AUTO_BUFFER\n");

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

  if (pipeline_descriptor_binding->id == 0) {
    pipeline_descriptor_binding->id = database_get_sequence_index_by_name("PIPELINE_DESCRIPTOR_BINDINGS");
  }
}

void database_store_model_asset(model_asset_t *model_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_ASSETS (NAME)\n");
  string_appendf(&sql, "VALUES (?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, model_asset->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (model_asset->id == 0) {
    model_asset->id = database_get_sequence_index_by_name("MODEL_ASSETS");
  }
}
void database_store_model_resource(model_resource_t *model_resource) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_RESOURCES (MODEL_ASSET_ID, MODEL_FILE_PATH)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (MODEL_ASSET_ID) DO UPDATE SET\n");
  string_appendf(&sql, "MODEL_FILE_PATH = EXCLUDED.MODEL_FILE_PATH\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_resource->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_resource->model_file_path, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (model_resource->id == 0) {
    model_resource->id = database_get_sequence_index_by_name("MODEL_RESOURCES");
  }
}
void database_store_model_mesh(model_mesh_t *model_mesh) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MODEL_MESHES (MODEL_ASSET_ID, NAME)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");
  string_appendf(&sql, "ON CONFLICT (MODEL_ASSET_ID) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, model_mesh->model_asset_id);
  sqlite3_bind_text(stmt, 2, model_mesh->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (model_mesh->id == 0) {
    model_mesh->id = database_get_sequence_index_by_name("MODEL_MESHES");
  }
}

void database_store_mesh_primitive(mesh_primitive_t *mesh_primitive) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MESH_PRIMITIVES (MODEL_MESH_ID, NAME)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_primitive->model_mesh_id);
  sqlite3_bind_text(stmt, 2, mesh_primitive->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (mesh_primitive->id == 0) {
    mesh_primitive->id = database_get_sequence_index_by_name("MESH_PRIMITIVES");
  }
}
void database_store_mesh_attribute(mesh_attribute_t *mesh_attribute) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO MESH_ATTRIBUTES (MESH_PRIMITIVE_ID, NAME, TYPE, COUNT)\n");
  string_appendf(&sql, "VALUES (?, ?, ?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, mesh_attribute->mesh_primitive_id);
  sqlite3_bind_text(stmt, 2, mesh_attribute->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, mesh_attribute->type);
  sqlite3_bind_int64(stmt, 4, mesh_attribute->count);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (mesh_attribute->id == 0) {
    mesh_attribute->id = database_get_sequence_index_by_name("MESH_ATTRIBUTES");
  }
}

void database_store_attribute_buffer(attribute_buffer_t *attribute_buffer) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO ATTRIBUTE_BUFFER (MESH_ATTRIBUTE_ID, DATA)\n");
  string_appendf(&sql, "VALUES (?, ?)\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_int64(stmt, 1, attribute_buffer->mesh_attribute_id);
  sqlite3_bind_blob(stmt, 2, attribute_buffer->data, (int32_t)attribute_buffer->data_size, SQLITE_STATIC);
  sqlite3_bind_int64(stmt, 3, attribute_buffer->data_size);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (attribute_buffer->id == 0) {
    attribute_buffer->id = database_get_sequence_index_by_name("ATTRIBUTE_BUFFER");
  }
}

void database_store_scene_asset(scene_asset_t *scene_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO SCENE_ASSETS (NAME)\n");
  string_appendf(&sql, "VALUES (?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, scene_asset->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (scene_asset->id == 0) {
    scene_asset->id = database_get_sequence_index_by_name("SCENE_ASSETS");
  }
}

void database_store_graph_asset(graph_asset_t *graph_asset) {
  string_t sql = string_create();

  string_appendf(&sql, "INSERT INTO GRAPH_ASSETS (NAME)\n");
  string_appendf(&sql, "VALUES (?)\n");
  string_appendf(&sql, "ON CONFLICT (NAME) DO NOTHING\n");

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, graph_asset->name, -1, SQLITE_STATIC);

  SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  if (graph_asset->id == 0) {
    graph_asset->id = database_get_sequence_index_by_name("GRAPH_ASSETS");
  }
}

void database_destroy_swapchain_assets(vector_t *swapchain_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(swapchain_assets);

  while (asset_index < asset_count) {

    swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(swapchain_assets, asset_index);

    heap_free(swapchain_asset->name);

    asset_index++;
  }

  vector_destroy(swapchain_assets);
}
void database_destroy_swapchain_asset(swapchain_asset_t *swapchain_asset) {
  heap_free(swapchain_asset->name);
}

void database_destroy_renderer_assets(vector_t *renderer_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(renderer_assets);

  while (asset_index < asset_count) {

    renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(renderer_assets, asset_index);

    heap_free(renderer_asset->name);

    asset_index++;
  }

  vector_destroy(renderer_assets);
}
void database_destroy_renderer_asset(renderer_asset_t *renderer_asset) {
  heap_free(renderer_asset->name);
}

void database_destroy_pipeline_assets(vector_t *pipeline_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(pipeline_assets);

  while (asset_index < asset_count) {

    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(pipeline_assets, asset_index);

    heap_free(pipeline_asset->name);

    asset_index++;
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
  uint64_t binding_index = 0;
  uint64_t binding_count = vector_count(vertex_input_bindings);

  while (binding_index < binding_count) {

    pipeline_vertex_input_binding_t *vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(vertex_input_bindings, binding_index);

    heap_free(vertex_input_binding->binding_name);

    binding_index++;
  }

  vector_destroy(vertex_input_bindings);
}
void database_destroy_pipeline_descriptor_bindings(vector_t *descriptor_bindings) {
  uint64_t binding_index = 0;
  uint64_t binding_count = vector_count(descriptor_bindings);

  while (binding_index < binding_count) {

    pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(descriptor_bindings, binding_index);

    heap_free(pipeline_descriptor_binding->binding_name);

    binding_index++;
  }

  vector_destroy(descriptor_bindings);
}

void database_destroy_model_assets(vector_t *model_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(model_assets);

  while (asset_index < asset_count) {

    model_asset_t *model_asset = (model_asset_t *)vector_at(model_assets, asset_index);

    heap_free(model_asset->name);

    asset_index++;
  }

  vector_destroy(model_assets);
}
void database_destroy_model_resource(model_resource_t *model_resource) {
  if (model_resource->model_file_path) {
    heap_free(model_resource->model_file_path);
  }
}
void database_destroy_model_meshes(vector_t *model_meshes) {
  uint64_t mesh_index = 0;
  uint64_t mesh_count = vector_count(model_meshes);

  while (mesh_index < mesh_count) {

    model_mesh_t *model_mesh = (model_mesh_t *)vector_at(model_meshes, mesh_index);

    heap_free(model_mesh->name);

    mesh_index++;
  }

  vector_destroy(model_meshes);
}

void database_destroy_mesh_primitives(vector_t *mesh_primitives) {
  uint64_t primitive_index = 0;
  uint64_t primitive_count = vector_count(mesh_primitives);

  while (primitive_index < primitive_count) {

    mesh_primitive_t *mesh_primitive = (mesh_primitive_t *)vector_at(mesh_primitives, primitive_index);

    heap_free(mesh_primitive->name);

    primitive_index++;
  }

  vector_destroy(mesh_primitives);
}
void database_destroy_mesh_attributes(vector_t *mesh_attributes) {
  uint64_t attribute_index = 0;
  uint64_t attribute_count = vector_count(mesh_attributes);

  while (attribute_index < attribute_count) {

    mesh_attribute_t *mesh_attribute = (mesh_attribute_t *)vector_at(mesh_attributes, attribute_index);

    heap_free(mesh_attribute->name);

    attribute_index++;
  }

  vector_destroy(mesh_attributes);
}

void database_destroy_attribute_buffer(attribute_buffer_t *attribute_buffer) {
  heap_free(attribute_buffer->data);
}

void database_destroy_scene_assets(vector_t *scene_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(scene_assets);

  while (asset_index < asset_count) {

    scene_asset_t *scene_asset = (scene_asset_t *)vector_at(scene_assets, asset_index);

    heap_free(scene_asset->name);

    asset_index++;
  }

  vector_destroy(scene_assets);
}

void database_destroy_graph_assets(vector_t *graph_assets) {
  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(graph_assets);

  while (asset_index < asset_count) {

    graph_asset_t *graph_asset = (graph_asset_t *)vector_at(graph_assets, asset_index);

    heap_free(graph_asset->name);

    asset_index++;
  }

  vector_destroy(graph_assets);
}

static int64_t database_get_sequence_index_by_name(char const *table_name) {
  int64_t sequence_id = 1;

  string_t sql = string_create();

  string_appendf(&sql, "SELECT seq FROM sqlite_sequence WHERE name = '%s'\n", table_name);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    sequence_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return sequence_id;
}
