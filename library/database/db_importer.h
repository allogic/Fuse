#ifndef DB_IMPORTER_H
#define DB_IMPORTER_H

#include <stdint.h>

#include <library/database/db_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void db_importer_insert_graphic_pipeline(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path);
void db_importer_insert_compute_pipeline(char const *pipeline_name, char const *compute_shader_file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DB_IMPORTER_H
