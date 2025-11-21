#include <library/database/lb_pch.h>
#include <library/database/lb_database.h>

static int64_t lb_database_get_sequence_index_by_name(char const *table_name);

static sqlite3 *s_database_handle = 0;

void lb_database_create(void) {
  LB_SQL_CHECK(sqlite3_open(LB_FILE_PATH, &s_database_handle));
}
void lb_database_destroy(void) {
  LB_SQL_CHECK(sqlite3_close(s_database_handle));
}

lb_scene_id_t lb_database_load_default_scene_id(void) {
  lb_scene_id_t scene_id = -1;

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT SCENE.ID\n");
  lb_string_appendf(&sql, "FROM SCENE\n");
  lb_string_appendf(&sql, "WHERE SCENE.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    scene_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return scene_id;
}
lb_swapchain_id_t lb_database_load_default_swapchain_id(void) {
  lb_swapchain_id_t swapchain_id = -1;

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT SWAPCHAIN.ID\n");
  lb_string_appendf(&sql, "FROM SWAPCHAIN\n");
  lb_string_appendf(&sql, "WHERE SWAPCHAIN.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    swapchain_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return swapchain_id;
}
lb_renderer_id_t lb_database_load_default_renderer_id(void) {
  lb_renderer_id_t renderer_id = -1;

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT RENDERER.ID\n");
  lb_string_appendf(&sql, "FROM RENDERER\n");
  lb_string_appendf(&sql, "WHERE RENDERER.IS_DEFAULT = 1\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    renderer_id = sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return renderer_id;
}

lb_vector_t lb_database_load_all_swapchains(void) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_swapchain_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT SWAPCHAIN.ID, SWAPCHAIN.NAME, SWAPCHAIN.IMAGE_COUNT, SWAPCHAIN.DEPTH_FORMAT, SWAPCHAIN.IS_DEFAULT\n");
  lb_string_appendf(&sql, "FROM SWAPCHAIN\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_swapchain_t swapchain = {0};

    lb_swapchain_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    int32_t depth_format = sqlite3_column_int(stmt, 3);
    uint8_t is_default = sqlite3_column_int(stmt, 4);

    uint64_t name_size = strlen(name) + 1;

    swapchain.id = id;
    swapchain.name = heap_alloc(name_size, 1, name);
    swapchain.name_size = name_size;
    swapchain.image_count = image_count;
    swapchain.depth_format = depth_format;
    swapchain.is_default = is_default;

    lb_vector_push(&vector, &swapchain);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_swapchain_t lb_database_load_swapchain_by_id(lb_swapchain_id_t swapchain_id) {
  lb_swapchain_t swapchain = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT SWAPCHAIN.ID, SWAPCHAIN.NAME, SWAPCHAIN.IMAGE_COUNT, SWAPCHAIN.DEPTH_FORMAT, SWAPCHAIN.IS_DEFAULT\n");
  lb_string_appendf(&sql, "FROM SWAPCHAIN\n");
  lb_string_appendf(&sql, "WHERE SWAPCHAIN.ID = %lld\n", swapchain_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_swapchain_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t image_count = sqlite3_column_int(stmt, 2);
    int32_t depth_format = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    swapchain.id = id;
    swapchain.name = lb_heap_alloc(name_size, 1, name);
    swapchain.name_size = name_size;
    swapchain.image_count = image_count;
    swapchain.depth_format = depth_format;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return swapchain;
}

lb_vector_t lb_database_load_all_renderer(void) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_renderer_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT RENDERER.ID, RENDERER.NAME, RENDERER.FRAMES_IN_FLIGHT, RENDERER.IS_DEFAULT\n");
  lb_string_appendf(&sql, "FROM RENDERER\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_t renderer = {0};

    lb_renderer_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    renderer.id = id;
    renderer.name = lb_heap_alloc(name_size, 1, name);
    renderer.name_size = name_size;
    renderer.frames_in_flight = frames_in_flight;
    renderer.is_default = is_default;

    lb_vector_push(&vector, &renderer);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_renderer_t lb_database_load_renderer_by_id(lb_renderer_id_t renderer_id) {
  lb_renderer_t renderer = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT RENDERER.ID, RENDERER.NAME, RENDERER.FRAMES_IN_FLIGHT, RENDERER.IS_DEFAULT\n");
  lb_string_appendf(&sql, "FROM RENDERER\n");
  lb_string_appendf(&sql, "WHERE RENDERER.ID = %lld\n", renderer_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    int32_t frames_in_flight = sqlite3_column_int(stmt, 2);
    uint8_t is_default = sqlite3_column_int(stmt, 3);

    uint64_t name_size = strlen(name) + 1;

    renderer.id = id;
    renderer.name = lb_heap_alloc(name_size, 1, name);
    renderer.name_size = name_size;
    renderer.frames_in_flight = frames_in_flight;
    renderer.is_default = is_default;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return renderer;
}

uint64_t lb_database_load_vertex_input_binding_count_by_id(lb_pipeline_id_t pipeline_id) {
  uint64_t pipeline_vertex_input_binding_count = 0;

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT COUNT(*) AS CNT\n");
  lb_string_appendf(&sql, "FROM PIPELINE_VERTEX_INPUT_BINDING\n");
  lb_string_appendf(&sql, "WHERE PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ID = %lld\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_vertex_input_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return pipeline_vertex_input_binding_count;
}
uint64_t lb_database_load_descriptor_binding_count_by_id(lb_pipeline_id_t pipeline_id) {
  uint64_t pipeline_descriptor_binding_count = 0;

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT COUNT(*) AS CNT\n");
  lb_string_appendf(&sql, "FROM PIPELINE_DESCRIPTOR_BINDING\n");
  lb_string_appendf(&sql, "WHERE PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ASSET_ID = %lld\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    pipeline_descriptor_binding_count = (uint64_t)sqlite3_column_int64(stmt, 0);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return pipeline_descriptor_binding_count;
}

lb_vector_t lb_database_load_all_pipelines(void) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_pipeline_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT PIPELINE.ID, PIPELINE.NAME, PIPELINE.TYPE, PIPELINE.LINK_INDEX, PIPELINE.AUTO_CREATE_PIPELINE, PIPELINE.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  lb_string_appendf(&sql, "FROM PIPELINE\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_t pipeline = {0};

    lb_pipeline_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    uint64_t name_size = strlen(name) + 1;

    pipeline.id = id;
    pipeline.name = lb_heap_alloc(name_size, 1, name);
    pipeline.name_size = name_size;
    pipeline.type = type;
    pipeline.link_index = link_index;
    pipeline.auto_create_pipeline = auto_create_pipeline;
    pipeline.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    lb_vector_push(&vector, &pipeline);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_vector_t lb_database_load_all_pipeline_by_type(lb_pipeline_type_t pipeline_type) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_pipeline_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT PIPELINE.ID, PIPELINE.NAME, PIPELINE.TYPE, PIPELINE.LINK_INDEX, PIPELINE.AUTO_CREATE_PIPELINE, PIPELINE.AUTO_CREATE_VERTEX_INPUT_BUFFER, PIPELINE.AUTO_LINK_DESCRIPTOR_BINDINGS, PIPELINE.INTERLEAVED_VERTEX_INPUT_BUFFER\n");
  lb_string_appendf(&sql, "FROM PIPELINE\n");
  lb_string_appendf(&sql, "WHERE TYPE = %u\n", pipeline_type);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_t pipeline = {0};

    lb_pipeline_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t link_index = (uint32_t)sqlite3_column_int(stmt, 3);
    uint8_t auto_create_pipeline = (uint8_t)sqlite3_column_int(stmt, 4);
    uint8_t auto_create_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_link_descriptor_bindings = (uint8_t)sqlite3_column_int(stmt, 6);
    uint8_t interleaved_vertex_input_buffer = (uint8_t)sqlite3_column_int(stmt, 7);

    uint64_t name_size = strlen(name) + 1;

    pipeline.id = id;
    pipeline.name = lb_heap_alloc(name_size, 1, name);
    pipeline.name_size = name_size;
    pipeline.type = type;
    pipeline.link_index = link_index;
    pipeline.auto_create_pipeline = auto_create_pipeline;
    pipeline.auto_create_vertex_input_buffer = auto_create_vertex_input_buffer;
    pipeline.auto_link_descriptor_bindings = auto_link_descriptor_bindings;
    pipeline.interleaved_vertex_input_buffer = interleaved_vertex_input_buffer;

    lb_vector_push(&vector, &pipeline);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_pipeline_resource_t lb_database_load_pipeline_resource_by_id(lb_pipeline_id_t pipeline_id) {
  lb_pipeline_resource_t pipeline_resource = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT PIPELINE_RESOURCE.ID, PIPELINE_RESOURCE.VERTEX_SHADER_FILE_PATH, PIPELINE_RESOURCE.FRAGMENT_SHADER_FILE_PATH, PIPELINE_RESOURCE.COMPUTE_SHADER_FILE_PATH\n");
  lb_string_appendf(&sql, "FROM PIPELINE\n");
  lb_string_appendf(&sql, "LEFT JOIN PIPELINE_RESOURCE\n");
  lb_string_appendf(&sql, "ON PIPELINE.ID = PIPELINE_RESOURCE.PIPELINE_ID\n");
  lb_string_appendf(&sql, "WHERE PIPELINE_RESOURCE.ID = %lld\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *vertex_shader_file_path = sqlite3_column_text(stmt, 1);
    char const *fragment_shader_file_path = sqlite3_column_text(stmt, 2);
    char const *compute_shader_file_path = sqlite3_column_text(stmt, 3);

    uint64_t vertex_shader_file_path_size = vertex_shader_file_path ? strlen(vertex_shader_file_path) + 1 : 0;
    uint64_t fragment_shader_file_path_size = fragment_shader_file_path ? strlen(fragment_shader_file_path) + 1 : 0;
    uint64_t compute_shader_file_path_size = compute_shader_file_path ? strlen(compute_shader_file_path) + 1 : 0;

    pipeline_resource.id = id;
    pipeline_resource.pipeline_id = pipeline_id;
    pipeline_resource.vertex_shader_file_path = vertex_shader_file_path ? lb_heap_alloc(vertex_shader_file_path_size, 1, vertex_shader_file_path) : 0;
    pipeline_resource.fragment_shader_file_path = fragment_shader_file_path ? lb_heap_alloc(fragment_shader_file_path_size, 1, fragment_shader_file_path) : 0;
    pipeline_resource.compute_shader_file_path = compute_shader_file_path ? lb_heap_alloc(compute_shader_file_path_size, 1, compute_shader_file_path) : 0;
    pipeline_resource.vertex_shader_file_path_size = vertex_shader_file_path_size;
    pipeline_resource.fragment_shader_file_path_size = fragment_shader_file_path_size;
    pipeline_resource.compute_shader_file_path_size = compute_shader_file_path_size;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return pipeline_resource;
}
lb_vector_t lb_database_load_all_pipeline_vertex_input_bindings_by_id(lb_pipeline_id_t pipeline_id) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_pipeline_vertex_input_binding_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT PIPELINE_VERTEX_INPUT_BINDING.ID, PIPELINE_VERTEX_INPUT_BINDING.BINDING_NAME, PIPELINE_VERTEX_INPUT_BINDING.LOCATION, PIPELINE_VERTEX_INPUT_BINDING.SIZE, PIPELINE_VERTEX_INPUT_BINDING.COMPONENT_COUNT, PIPELINE_VERTEX_INPUT_BINDING.FORMAT, PIPELINE_VERTEX_INPUT_BINDING.INPUT_RATE\n");
  lb_string_appendf(&sql, "FROM PIPELINE\n");
  lb_string_appendf(&sql, "LEFT JOIN PIPELINE_VERTEX_INPUT_BINDING\n");
  lb_string_appendf(&sql, "ON PIPELINE.ID = PIPELINE_VERTEX_INPUT_BINDING.PIPELINE_ID\n");
  lb_string_appendf(&sql, "WHERE PIPELINE.ID = %lld\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_vertex_input_binding_t pipeline_vertex_input_binding = {0};

    lb_pipeline_vertex_input_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *binding_name = sqlite3_column_text(stmt, 1);
    uint32_t location = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t size = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t component_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t format = (uint32_t)sqlite3_column_int(stmt, 5);
    uint32_t input_rate = (uint32_t)sqlite3_column_int(stmt, 6);

    uint64_t binding_name_size = binding_name ? strlen(binding_name) + 1 : 0;

    pipeline_vertex_input_binding.id = id;
    pipeline_vertex_input_binding.pipeline_id = pipeline_id;
    pipeline_vertex_input_binding.binding_name = lb_heap_alloc(binding_name_size, 1, binding_name);
    pipeline_vertex_input_binding.binding_name_size = binding_name_size;
    pipeline_vertex_input_binding.location = location;
    pipeline_vertex_input_binding.size = size;
    pipeline_vertex_input_binding.component_count = component_count;
    pipeline_vertex_input_binding.format = format;
    pipeline_vertex_input_binding.input_rate = input_rate;

    lb_vector_push(&vector, &pipeline_vertex_input_binding);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_vector_t lb_database_load_all_pipeline_descriptor_bindings_by_id(lb_pipeline_id_t pipeline_id) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_pipeline_descriptor_binding_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT PIPELINE_DESCRIPTOR_BINDING.ID, PIPELINE_DESCRIPTOR_BINDING.BINDING_NAME, PIPELINE_DESCRIPTOR_BINDING.BINDING, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_TYPE, PIPELINE_DESCRIPTOR_BINDING.DESCRIPTOR_COUNT, PIPELINE_DESCRIPTOR_BINDING.STAGE_FLAGS, PIPELINE_DESCRIPTOR_BINDING.AUTO_BUFFER\n");
  lb_string_appendf(&sql, "FROM PIPELINE\n");
  lb_string_appendf(&sql, "LEFT JOIN PIPELINE_DESCRIPTOR_BINDING\n");
  lb_string_appendf(&sql, "ON PIPELINE.ID = PIPELINE_DESCRIPTOR_BINDING.PIPELINE_ID\n");
  lb_string_appendf(&sql, "WHERE PIPELINE.ID = %lld\n", pipeline_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_pipeline_descriptor_binding_t pipeline_descriptor_binding = {0};

    lb_pipeline_descriptor_binding_id_t id = sqlite3_column_int64(stmt, 0);
    char const *binding_name = (char const *)sqlite3_column_text(stmt, 1);
    uint32_t binding = (uint32_t)sqlite3_column_int(stmt, 2);
    uint32_t descriptor_type = (uint32_t)sqlite3_column_int(stmt, 3);
    uint32_t descriptor_count = (uint32_t)sqlite3_column_int(stmt, 4);
    uint32_t stage_flags = (uint32_t)sqlite3_column_int(stmt, 5);
    uint8_t auto_buffer = (uint8_t)sqlite3_column_int(stmt, 6);

    uint64_t binding_name_size = binding_name ? strlen(binding_name) + 1 : 0;

    pipeline_descriptor_binding.id = id;
    pipeline_descriptor_binding.pipeline_id = pipeline_id;
    pipeline_descriptor_binding.binding_name = lb_heap_alloc(binding_name_size, 1, binding_name);
    pipeline_descriptor_binding.binding_name_size = binding_name_size;
    pipeline_descriptor_binding.binding = binding;
    pipeline_descriptor_binding.descriptor_type = descriptor_type;
    pipeline_descriptor_binding.descriptor_count = descriptor_count;
    pipeline_descriptor_binding.stage_flags = stage_flags;
    pipeline_descriptor_binding.auto_buffer = auto_buffer;

    lb_vector_push(&vector, &pipeline_descriptor_binding);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}

lb_vector_t lb_database_load_all_models(void) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_pipeline_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT MODEL.ID, MODEL.NAME\n");
  lb_string_appendf(&sql, "FROM MODEL\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_t model = {0};

    lb_model_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    model.id = id;
    model.name = lb_heap_alloc(name_size, 1, name);
    model.name_size = name_size;

    lb_vector_push(&vector, &model);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_model_resource_t lb_database_load_model_resource_by_id(lb_model_id_t model_id) {
  lb_model_resource_t model_resource = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT MODEL_RESOURCE.ID, MODEL_RESOURCE.MODEL_FILE_PATH\n");
  lb_string_appendf(&sql, "FROM MODEL\n");
  lb_string_appendf(&sql, "LEFT JOIN MODEL_RESOURCE\n");
  lb_string_appendf(&sql, "ON MODEL.ID = MODEL_RESOURCE.MODEL_ID\n");
  lb_string_appendf(&sql, "WHERE MODEL.ID = %lld\n", model_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_resource_id_t id = sqlite3_column_int64(stmt, 0);
    char const *model_file_path = sqlite3_column_text(stmt, 1);

    uint64_t model_file_path_size = model_file_path ? strlen(model_file_path) + 1 : 0;

    model_resource.id = id;
    model_resource.model_id = model_id;
    model_resource.model_file_path = model_file_path ? lb_heap_alloc(model_file_path_size, 1, model_file_path) : 0;
    model_resource.model_file_path_size = model_file_path_size;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return model_resource;
}
lb_vector_t lb_database_load_all_model_meshes_by_id(lb_model_id_t model_id) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_model_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT MODEL_MESH.ID, MODEL_MESH.NAME\n");
  lb_string_appendf(&sql, "FROM MODEL\n");
  lb_string_appendf(&sql, "LEFT JOIN MODEL_MESH\n");
  lb_string_appendf(&sql, "ON MODEL.ID = MODEL_MESH.MODEL_ID\n");
  lb_string_appendf(&sql, "WHERE MODEL.ID = %lld\n", model_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_model_mesh_t model_mesh = {0};

    lb_model_mesh_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    model_mesh.id = id;
    model_mesh.model_id = model_id;
    model_mesh.name = lb_heap_alloc(name_size, 1, name);
    model_mesh.name_size = name_size;

    lb_vector_push(&vector, &model_mesh);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}

lb_vector_t lb_database_load_all_mesh_primitives_by_id(lb_model_mesh_id_t model_mesh_id) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_mesh_primitive_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT MESH_PRIMITIVE.ID, MESH_PRIMITIVE.NAME\n");
  lb_string_appendf(&sql, "FROM MODEL\n");
  lb_string_appendf(&sql, "LEFT JOIN MESH_PRIMITIVE\n");
  lb_string_appendf(&sql, "ON MODEL.ID = MESH_PRIMITIVE.MODEL_MESH_ID\n");
  lb_string_appendf(&sql, "WHERE MODEL.ID = %lld\n", model_mesh_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_mesh_primitive_t mesh_primitive = {0};

    lb_mesh_primitive_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    mesh_primitive.id = id;
    mesh_primitive.model_mesh_id = model_mesh_id;
    mesh_primitive.name = lb_heap_alloc(name_size, 1, name);
    mesh_primitive.name_size = name_size;

    lb_vector_push(&vector, &mesh_primitive);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}
lb_vector_t lb_database_load_all_mesh_attributes_by_id(lb_mesh_primitive_id_t mesh_primitive_id) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_mesh_attribute_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT MESH_ATTRIBUTE.ID, MESH_ATTRIBUTE.NAME, MESH_ATTRIBUTE.TYPE, MESH_ATTRIBUTE.COUNT\n");
  lb_string_appendf(&sql, "FROM MESH_PRIMITIVE\n");
  lb_string_appendf(&sql, "LEFT JOIN MESH_ATTRIBUTE\n");
  lb_string_appendf(&sql, "ON MESH_PRIMITIVE.ID = MESH_ATTRIBUTE.MESH_PRIMITIVE_ID\n");
  lb_string_appendf(&sql, "WHERE MESH_PRIMITIVE.ID = %lld\n", mesh_primitive_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_mesh_attribute_t mesh_attribute = {0};

    lb_mesh_attribute_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);
    uint32_t type = sqlite3_column_int(stmt, 2);
    uint64_t count = sqlite3_column_int64(stmt, 3);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    mesh_attribute.id = id;
    mesh_attribute.mesh_primitive_id = mesh_primitive_id;
    mesh_attribute.name = lb_heap_alloc(name_size, 1, name);
    mesh_attribute.name_size = name_size;
    mesh_attribute.type = type;
    mesh_attribute.count = count;

    lb_vector_push(&vector, &mesh_attribute);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}

lb_attribute_buffer_t lb_database_load_attribute_buffer_by_id(lb_mesh_attribute_id_t mesh_attribute_id) {
  lb_attribute_buffer_t attribute_buffer = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT ATTRIBUTE_BUFFER.ID, ATTRIBUTE_BUFFER.DATA\n");
  lb_string_appendf(&sql, "FROM MESH_ATTRIBUTE\n");
  lb_string_appendf(&sql, "LEFT JOIN ATTRIBUTE_BUFFER\n");
  lb_string_appendf(&sql, "ON MESH_ATTRIBUTE.ID = ATTRIBUTE_BUFFER.MESH_ATTRIBUTE_ID\n");
  lb_string_appendf(&sql, "WHERE MESH_ATTRIBUTE.ID = %lld\n", mesh_attribute_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

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

  lb_string_destroy(&sql);

  return attribute_buffer;
}

lb_vector_t lb_database_load_all_scenes(void) {
  lb_vector_t vector = lb_vector_create(sizeof(lb_scene_t));

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT SCENE.ID, SCENE.NAME\n");
  lb_string_appendf(&sql, "FROM SCENE\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_scene_t scene = {0};

    lb_scene_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = name ? strlen(name) + 1 : 0;

    scene.id = id;
    scene.name = lb_heap_alloc(name_size, 1, name);
    scene.name_size = name_size;

    lb_vector_push(&vector, &scene);
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return vector;
}

lb_graph_t lb_database_load_graph_by_id(lb_graph_id_t graph_id) {
  lb_graph_t graph = {0};

  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "SELECT GRAPH.ID, GRAPH.NAME\n");
  lb_string_appendf(&sql, "FROM GRAPH\n");
  lb_string_appendf(&sql, "WHERE GRAPH.ID = %lld\n", graph_id);

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {

    lb_renderer_id_t id = sqlite3_column_int64(stmt, 0);
    char const *name = sqlite3_column_text(stmt, 1);

    uint64_t name_size = strlen(name) + 1;

    graph.id = id;
    graph.name = lb_heap_alloc(name_size, 1, name);
    graph.name_size = name_size;
  }

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  return graph;
}

void lb_database_store_swapchain(lb_swapchain_t *swapchain) {
  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "INSERT INTO SWAPCHAIN (NAME, IMAGE_COUNT, DEPTH_FORMAT)\n");
  lb_string_appendf(&sql, "VALUES (?, ?, ?)\n");
  lb_string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  lb_string_appendf(&sql, "IMAGE_COUNT = EXCLUDED.IMAGE_COUNT,\n");
  lb_string_appendf(&sql, "DEPTH_FORMAT = EXCLUDED.DEPTH_FORMAT\n");

  sqlite3_stmt *stmt = 0;

  LB_SQL_CHECK(sqlite3_prepare_v2(s_database_handle, lb_string_buffer(&sql), -1, &stmt, 0));

  sqlite3_bind_text(stmt, 1, swapchain->name, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, swapchain->image_count);
  sqlite3_bind_int(stmt, 3, swapchain->depth_format);

  LB_SQL_CHECK_STATUS(sqlite3_step(stmt), SQLITE_DONE);

  sqlite3_finalize(stmt);

  lb_string_destroy(&sql);

  if (swapchain->id == 0) {
    swapchain->id = database_get_sequence_index_by_name("SWAPCHAIN");
  }
}

void lb_database_store_renderer_asset(lb_renderer_t *renderer) {
  lb_string_t sql = lb_string_create();

  lb_string_appendf(&sql, "INSERT INTO RENDERER (NAME, FRAMES_IN_FLIGHT)\n");
  lb_string_appendf(&sql, "VALUES (?, ?)\n");
  lb_string_appendf(&sql, "ON CONFLICT (NAME) DO UPDATE SET\n");
  lb_string_appendf(&sql, "FRAMES_IN_FLIGHT = EXCLUDED.FRAMES_IN_FLIGHT\n");

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
