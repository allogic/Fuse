#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>

#include <engine/pipeline.h>

#include <sqlite3/sqlite3.h>

typedef enum database_fetch_flags_t {
  DATABASE_FETCH_NONE = 0,
  DATABASE_FETCH_RENEW,
} database_fetch_flags_t;

typedef struct database_pipeline_record_t {
  char const *name;
  pipeline_type_t type;
} database_pipeline_record_t;

typedef struct database_pipelines_t {
  sqlite3_stmt *statement;
  vector_t records;
} database_pipelines_t;

typedef struct database_graphic_pipeline_settings_t {
  sqlite3_stmt *statement;
  char const *pipeline_name;
  uint8_t *vertex_shader;
  uint8_t *fragment_shader;
  uint64_t vertex_shader_size;
  uint64_t fragment_shader_size;
} database_graphic_pipeline_settings_t;

typedef struct database_compute_pipeline_settings_t {
  sqlite3_stmt *statement;
  char const *pipeline_name;
  uint8_t *compute_shader;
  uint64_t compute_shader_size;
} database_compute_pipeline_settings_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void database_create(void);
void database_destroy(void);

database_pipelines_t database_fetch_pipelines_by_type(pipeline_type_t pipeline_type);
void database_close_pipelines(database_pipelines_t *pipelines);

database_graphic_pipeline_settings_t database_fetch_graphic_pipeline_settings_by_name(char const *pipeline_name);
void database_close_graphic_pipeline_settings(database_graphic_pipeline_settings_t *settings);

database_compute_pipeline_settings_t database_fetch_compute_pipeline_settings_by_name(char const *pipeline_name);
void database_close_compute_pipeline_settings(database_compute_pipeline_settings_t *settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DATABASE_H
