#ifndef LB_IMPORTER_IMPORTER_H
#define LB_IMPORTER_IMPORTER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void importer_import_default_assets(void);

void importer_import_graphic_pipeline(graphic_pipeline_import_settings_t *import_settings);
void importer_import_compute_pipeline(compute_pipeline_import_settings_t *import_settings);

void importer_import_model(model_import_settings_t *import_settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_IMPORTER_IMPORTER_H
