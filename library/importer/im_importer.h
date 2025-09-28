#ifndef im_IMPORTER_H
#define im_IMPORTER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void importer_import_graphic_pipeline(char const *pipeline_name, char const *vertex_shader_file_path, char const *fragment_shader_file_path);
void importer_import_compute_pipeline(char const *pipeline_name, char const *compute_shader_file_path);

void importer_import_model(char const *model_name, char const *model_file_path);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // IM_IMPORTER_H
