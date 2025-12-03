#ifndef EG_IMPORTER_H
#define EG_IMPORTER_H

#define EG_ASSET_ROOT_DIR "..\\..\\..\\..\\asset" // TODO

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_importer_import_default_assets(void);

void eg_importer_import_graphic_pipeline(eg_graphic_pipeline_import_settings_t *import_settings);
void eg_importer_import_compute_pipeline(eg_compute_pipeline_import_settings_t *import_settings);

void eg_importer_import_model(eg_model_import_settings_t *import_settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_IMPORTER_H
