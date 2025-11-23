#ifndef LB_IMPORTER_H
#define LB_IMPORTER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void lb_importer_import_default_assets(void);

void lb_importer_import_graphic_pipeline(lb_graphic_pipeline_import_settings_t *import_settings);
void lb_importer_import_compute_pipeline(lb_compute_pipeline_import_settings_t *import_settings);

void lb_importer_import_model(lb_model_import_settings_t *import_settings);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_IMPORTER_H
