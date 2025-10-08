#ifndef IM_FORWARD_H
#define IM_FORWARD_H

typedef struct graphic_pipeline_import_settings_t {
  char *pipeline_name;
  char *vertex_shader_file_path;
  char *fragment_shader_file_path;
  uint8_t auto_create;
  uint8_t auto_vertex_input_buffer;
  uint8_t interleaved_vertex_input;
} graphic_pipeline_import_settings_t;

typedef struct compute_pipeline_import_settings_t {
  char *pipeline_name;
  char *compute_shader_file_path;
} compute_pipeline_import_settings_t;

typedef struct model_import_settings_t {
  char *model_name;
  char *model_file_path;
} model_import_settings_t;

#endif // IM_FORWARD_H
