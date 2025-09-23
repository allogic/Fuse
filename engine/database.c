#include <stdio.h>

#include <library/core/api.h>

#include <engine/database.h>
#include <engine/macros.h>

static sqlite3 *s_database_handle = 0;

void database_create(void) {
  SQL_CHECK(sqlite3_open("C:\\Users\\mialb\\Downloads\\fuse\\database\\data.db", &s_database_handle));
}
void database_destroy(void) {
  SQL_CHECK(sqlite3_close(s_database_handle));
}

database_pipelines_t database_fetch_pipelines_by_type(pipeline_type_t pipeline_type) {
  database_pipelines_t pipelines = {0};

  pipelines.records = vector_create(sizeof(database_pipeline_record_t));

  database_fetch_flags_t flags = DATABASE_FETCH_RENEW;

  if (flags & DATABASE_FETCH_RENEW) {
    string_t sql = string_create();

    string_appendf(&sql, "SELECT NAME FROM PIPELINES;");
    string_appendf(&sql, "WHERE TYPE = %d;", pipeline_type);

    sqlite3_stmt *stmt = 0;

    SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &stmt, 0));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      database_pipeline_record_t record = {0};

      record.name = (char const *)sqlite3_column_text(stmt, 0);
      record.type = (pipeline_type_t)sqlite3_column_int(stmt, 1);

      vector_push(&pipelines.records, &record);
    }

    sqlite3_finalize(stmt);

    string_destroy(&sql);
  }

  return pipelines;
}
void database_close_pipelines(database_pipelines_t *pipelines) {
  vector_destroy(&pipelines->records);

  sqlite3_finalize(pipelines->statement);
}

database_graphic_pipeline_settings_t database_fetch_graphic_pipeline_settings_by_name(char const *pipeline_name) {
  database_graphic_pipeline_settings_t settings = {0};

  database_fetch_flags_t flags = DATABASE_FETCH_RENEW;

  if (flags & DATABASE_FETCH_RENEW) {
    string_t sql = string_create();

    string_appendf(&sql, "SELECT GP.NAME, GPS.VERTEX_SHADER, GPS.FRAGMENT_SHADER, GPS.VERTEX_SHADER_SIZE, GPS.FRAGMENT_SHADER_SIZE");
    string_appendf(&sql, "  FROM GRAPHIC_PIPELINES AS GP");
    string_appendf(&sql, "  LEFT JOIN GRAPHIC_PIPELINE_SETTINGS AS GPS");
    string_appendf(&sql, "    ON GP.ID = GPS.PIPELINE_ID");
    string_appendf(&sql, "  WHERE GP.NAME = '%s'", pipeline_name);

    SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &settings.statement, 0));

    while (sqlite3_step(settings.statement) == SQLITE_ROW) {
      settings.pipeline_name = (char const *)sqlite3_column_text(settings.statement, 0);
      settings.vertex_shader = (uint8_t *)sqlite3_column_text(settings.statement, 1);
      settings.fragment_shader = (uint8_t *)sqlite3_column_text(settings.statement, 2);
      settings.vertex_shader_size = (uint64_t)sqlite3_column_int64(settings.statement, 3);
      settings.fragment_shader_size = (uint64_t)sqlite3_column_int64(settings.statement, 4);
    }

    string_destroy(&sql);
  }

  return settings;
}
void database_close_graphic_pipeline_settings(database_graphic_pipeline_settings_t *settings) {
  sqlite3_finalize(settings->statement);
}

database_compute_pipeline_settings_t database_fetch_compute_pipeline_settings_by_name(char const *pipeline_name) {
  database_compute_pipeline_settings_t settings = {0};

  database_fetch_flags_t flags = DATABASE_FETCH_RENEW;

  if (flags & DATABASE_FETCH_RENEW) {
    string_t sql = string_create();

    string_appendf(&sql, "SELECT CP.NAME, CPS.COMPUTE_SHADER, CPS.COMPUTE_SHADER_SIZE");
    string_appendf(&sql, "  FROM COMPUTE_PIPELINES AS CP");
    string_appendf(&sql, "  LEFT JOIN COMPUTE_PIPELINE_SETTINGS AS CPS");
    string_appendf(&sql, "    ON CP.ID = CPS.PIPELINE_ID");
    string_appendf(&sql, "  WHERE CP.NAME = '%s'", pipeline_name);

    SQL_CHECK(sqlite3_prepare_v2(s_database_handle, string_buffer(&sql), -1, &settings.statement, 0));

    while (sqlite3_step(settings.statement) == SQLITE_ROW) {
      settings.pipeline_name = (char const *)sqlite3_column_text(settings.statement, 0);
      settings.compute_shader = (uint8_t *)sqlite3_column_text(settings.statement, 0);
      settings.compute_shader_size = (uint64_t)sqlite3_column_int64(settings.statement, 1);
    }

    string_destroy(&sql);
  }

  return settings;
}
void database_close_compute_pipeline_settings(database_compute_pipeline_settings_t *settings) {
  sqlite3_finalize(settings->statement);
}
