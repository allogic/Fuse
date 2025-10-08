#ifndef im_IMPORTER_H
#define im_IMPORTER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void importer_import_graphic_pipeline(graphic_pipeline_import_settings_t *import_settings);
void importer_import_compute_pipeline(compute_pipeline_import_settings_t *import_settings);

void importer_import_model(model_import_settings_t *import_settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // IM_IMPORTER_H
