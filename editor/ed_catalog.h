#ifndef ED_CATALOG_H
#define ED_CATALOG_H

extern asset_type_t g_catalog_selected_asset_type;

extern vector_t g_catalog_swapchain_assets;
extern vector_t g_catalog_renderer_assets;
extern vector_t g_catalog_pipeline_assets;
extern vector_t g_catalog_model_assets;

extern int64_t g_catalog_selected_swapchain_asset;
extern int64_t g_catalog_selected_renderer_asset;
extern int64_t g_catalog_selected_pipeline_asset;
extern int64_t g_catalog_selected_model_asset;

extern vector_t g_catalog_pipeline_vertex_input_bindings;
extern vector_t g_catalog_pipeline_descriptor_bindings;

extern vector_t g_catalog_model_meshes;

void catalog_create();
void catalog_refresh();
void catalog_draw();
void catalog_destroy();

#endif // ED_CATALOG_H
