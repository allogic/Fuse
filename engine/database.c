#include <stdio.h>

#include <engine/database.h>
#include <engine/macros.h>

#include <sqlite3/sqlite3.h>

static sqlite3 *s_database_handle = 0;

static vector_t s_database_cached_graphic_pipelines = {0};
static vector_t s_database_cached_compute_pipelines = {0};
static vector_t s_database_cached_pipeline_bindings = {0};

static database_cache_flags_t s_database_graphic_pipeline_cache_flags = DATABASE_CACHE_FLAG_RENEW_BIT;
static database_cache_flags_t s_database_compute_pipeline_cache_flags = DATABASE_CACHE_FLAG_RENEW_BIT;
static database_cache_flags_t s_database_pipeline_binding_cache_flags = DATABASE_CACHE_FLAG_RENEW_BIT;

static void database_create_graphic_piplines(void);
static void database_create_compute_piplines(void);
static void database_create_pipeline_bindings(void);

static void database_destroy_graphic_piplines(void);
static void database_destroy_compute_piplines(void);
static void database_destroy_pipeline_bindings(void);

void database_create(void) {
  SQL_CHECK(sqlite3_open("C:\\Users\\mialb\\Downloads\\fuse\\database\\data.db", &s_database_handle));

  s_database_cached_graphic_pipelines = vector_create(sizeof(database_pipeline_t));
  s_database_cached_compute_pipelines = vector_create(sizeof(database_pipeline_t));
  s_database_cached_pipeline_bindings = vector_create(sizeof(database_pipeline_binding_t));
}
void database_destroy(void) {
  database_destroy_graphic_piplines();
  database_destroy_compute_piplines();

  vector_destroy(&s_database_cached_graphic_pipelines);
  vector_destroy(&s_database_cached_compute_pipelines);
  vector_destroy(&s_database_cached_pipeline_bindings);

  SQL_CHECK(sqlite3_close(s_database_handle));
}

vector_t *database_graphic_pipelines(void) {
  if (IS_BIT_SET(s_database_graphic_pipeline_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT)) {
    CLEAR_BIT(s_database_graphic_pipeline_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT);

    database_destroy_graphic_piplines();
    database_create_graphic_piplines();
  }

  return &s_database_cached_graphic_pipelines;
}
vector_t *database_compute_pipelines(void) {
  if (IS_BIT_SET(s_database_compute_pipeline_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT)) {
    CLEAR_BIT(s_database_compute_pipeline_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT);

    database_destroy_compute_piplines();
    database_create_compute_piplines();
  }

  return &s_database_cached_compute_pipelines;
}
vector_t *database_pipeline_bindings(void) {
  if (IS_BIT_SET(s_database_pipeline_binding_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT)) {
    CLEAR_BIT(s_database_pipeline_binding_cache_flags, DATABASE_CACHE_FLAG_RENEW_BIT);

    database_destroy_pipeline_bindings();
    database_create_pipeline_bindings();
  }

  return &s_database_cached_pipeline_bindings;
}

database_graphic_pipeline_settings_t database_load_graphic_pipeline_settings_by_name(char const *pipeline_name) {
  database_graphic_pipeline_settings_t settings = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT P.NAME, GPS.VERTEX_SHADER, GPS.FRAGMENT_SHADER\n");
  string_appendf(&sql, "FROM PIPELINES AS P\n");
  string_appendf(&sql, "LEFT JOIN GRAPHIC_PIPELINE_SETTINGS AS GPS\n");
  string_appendf(&sql, "ON P.ID = GPS.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE P.NAME = '%s'\n", pipeline_name);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    char const *pipeline_name = sqlite3_column_text(stmt, 0);
    uint8_t *vertex_shader = (uint8_t *)sqlite3_column_blob(stmt, 1);
    uint8_t *fragment_shader = (uint8_t *)sqlite3_column_blob(stmt, 2);
    uint64_t vertex_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 1);
    uint64_t fragment_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 2);

    settings.pipeline_name = string_create_from(pipeline_name);
    settings.vertex_shader = vector_create_from(sizeof(uint8_t), vertex_shader, vertex_shader_size);
    settings.fragment_shader = vector_create_from(sizeof(uint8_t), fragment_shader, fragment_shader_size);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return settings;
}
void database_destroy_graphic_pipeline_settings(database_graphic_pipeline_settings_t *settings) {
  string_destroy(&settings->pipeline_name);
  vector_destroy(&settings->vertex_shader);
  vector_destroy(&settings->fragment_shader);
}

database_compute_pipeline_settings_t database_load_compute_pipeline_settings_by_name(char const *pipeline_name) {
  database_compute_pipeline_settings_t settings = {0};

  string_t sql = string_create();

  string_appendf(&sql, "SELECT P.NAME, CPS.COMPUTE_SHADER\n");
  string_appendf(&sql, "FROM PIPELINES AS P\n");
  string_appendf(&sql, "LEFT JOIN COMPUTE_PIPELINE_SETTINGS AS CPS\n");
  string_appendf(&sql, "ON P.ID = CPS.PIPELINE_ID\n");
  string_appendf(&sql, "WHERE P.NAME = '%s'\n", pipeline_name);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    char const *pipeline_name = sqlite3_column_text(stmt, 0);
    uint8_t *compute_shader = (uint8_t *)sqlite3_column_blob(stmt, 1);
    uint64_t compute_shader_size = (uint64_t)sqlite3_column_bytes(stmt, 1);

    settings.pipeline_name = string_create_from(pipeline_name);
    settings.compute_shader = vector_create_from(sizeof(uint8_t), compute_shader, compute_shader_size);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);

  return settings;
}
void database_destroy_compute_pipeline_settings(database_compute_pipeline_settings_t *settings) {
  string_destroy(&settings->pipeline_name);
  vector_destroy(&settings->compute_shader);
}

static void database_create_graphic_piplines() {
  string_t sql = string_create();

  string_appendf(&sql, "SELECT NAME FROM PIPELINES WHERE TYPE = %d\n", PIPELINE_TYPE_GRAPHIC);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  vector_clear(&s_database_cached_graphic_pipelines);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    database_pipeline_t pipeline = {0};

    pipeline.name = string_create_from(sqlite3_column_text(stmt, 0));

    vector_push(&s_database_cached_graphic_pipelines, &pipeline);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
static void database_create_compute_piplines() {
  string_t sql = string_create();

  string_appendf(&sql, "SELECT NAME FROM PIPELINES WHERE TYPE = %d\n", PIPELINE_TYPE_COMPUTE);

  sqlite3_stmt *stmt = 0;

  SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

  vector_clear(&s_database_cached_compute_pipelines);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    database_pipeline_t pipeline = {0};

    pipeline.name = string_create_from(sqlite3_column_text(stmt, 0));

    vector_push(&s_database_cached_compute_pipelines, &pipeline);
  }

  sqlite3_finalize(stmt);

  string_destroy(&sql);
}
static void database_create_pipeline_bindings() {
}

static void database_destroy_graphic_piplines(void) {
  uint64_t pipeline_index = 0;
  uint64_t pipeline_count = vector_count(&s_database_cached_graphic_pipelines);

  while (pipeline_index < pipeline_count) {
    database_pipeline_t *pipeline = (database_pipeline_t *)vector_at(&s_database_cached_graphic_pipelines, pipeline_index);

    string_destroy(&pipeline->name);

    pipeline_index++;
  }
}
static void database_destroy_compute_piplines(void) {
  uint64_t pipeline_index = 0;
  uint64_t pipeline_count = vector_count(&s_database_cached_compute_pipelines);

  while (pipeline_index < pipeline_count) {
    database_pipeline_t *pipeline = (database_pipeline_t *)vector_at(&s_database_cached_compute_pipelines, pipeline_index);

    string_destroy(&pipeline->name);

    pipeline_index++;
  }
}
static void database_destroy_pipeline_bindings(void) {
}
