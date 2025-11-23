#ifndef LB_IMPORTER_FWD_H
#define LB_IMPORTER_FWD_H

typedef struct lb_graphic_pipeline_import_settings_t {
  char *pipeline_name;
  char *vertex_shader_file_path;
  char *fragment_shader_file_path;
  uint8_t auto_create_pipeline;
  uint8_t auto_create_vertex_input_buffer;
  uint8_t auto_link_descriptor_bindings;
  uint8_t interleaved_vertex_input;
} lb_graphic_pipeline_import_settings_t;

typedef struct lb_compute_pipeline_import_settings_t {
  char *pipeline_name;
  char *compute_shader_file_path;
} lb_compute_pipeline_import_settings_t;

typedef struct lb_model_import_settings_t {
  char *model_name;
  char *model_file_path;
} lb_model_import_settings_t;

#endif // LB_IMPORTER_FWD_H
