#ifndef DATABASE_H
#define DATABASE_H

#include <stdint.h>

#include <library/core/api.h>

#include <engine/pipeline.h>

typedef enum database_cache_flag_t {
  DATABASE_CACHE_FLAG_NONE = 0,
  DATABASE_CACHE_FLAG_RENEW_BIT,
} database_cache_flags_t;

typedef struct database_pipeline_t {
  string_t name;
} database_pipeline_t;

typedef struct database_pipeline_binding_t {
  string_t name;
  uint8_t no_auto_buffer;
} database_pipeline_binding_t;

typedef struct database_graphic_pipeline_settings_t {
  string_t pipeline_name;
  vector_t vertex_shader;
  vector_t fragment_shader;
} database_graphic_pipeline_settings_t;

typedef struct database_compute_pipeline_settings_t {
  string_t pipeline_name;
  vector_t compute_shader;
} database_compute_pipeline_settings_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void database_create(void);
void database_destroy(void);

vector_t *database_graphic_pipelines(void);
vector_t *database_compute_pipelines(void);
vector_t *database_pipeline_bindings(void);

database_graphic_pipeline_settings_t database_load_graphic_pipeline_settings_by_name(char const *pipeline_name);
void database_destroy_graphic_pipeline_settings(database_graphic_pipeline_settings_t *settings);

database_compute_pipeline_settings_t database_load_compute_pipeline_settings_by_name(char const *pipeline_name);
void database_destroy_compute_pipeline_settings(database_compute_pipeline_settings_t *settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DATABASE_H
