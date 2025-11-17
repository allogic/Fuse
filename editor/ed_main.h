#ifndef ED_MAIN_H
#define ED_MAIN_H

#define ED_DARK_GREY IM_COL32(31, 31, 31, 255)
#define ED_SPARSE_GRAY_COLOR IM_COL32(40, 40, 40, 255)
#define ED_SPARSE2_GRAY_COLOR IM_COL32(47, 47, 47, 255)
#define ED_SHALLOW_GRAY_COLOR IM_COL32(60, 60, 60, 255)
#define ED_LIGHT_GRAY_COLOR IM_COL32(77, 77, 77, 255)
#define ED_ACTIVE_GREY_COLOR IM_COL32(90, 90, 90, 255)

//////////////////////////////////////////////////////////////////////////////
// Editor Stuff
//////////////////////////////////////////////////////////////////////////////

extern VkDescriptorPool g_editor_descriptor_pool;

extern ImFont *g_editor_commit_mono_h1;
extern ImFont *g_editor_commit_mono_h2;
extern ImFont *g_editor_commit_mono_h3;
extern ImFont *g_editor_commit_mono_h4;
extern ImFont *g_editor_commit_mono_h5;
extern ImFont *g_editor_commit_mono_h6;
extern ImFont *g_editor_commit_mono;

extern ImFont *g_editor_material_symbols_h1;
extern ImFont *g_editor_material_symbols_h2;
extern ImFont *g_editor_material_symbols_h3;
extern ImFont *g_editor_material_symbols_h4;
extern ImFont *g_editor_material_symbols_h5;
extern ImFont *g_editor_material_symbols_h6;
extern ImFont *g_editor_material_symbols;

//////////////////////////////////////////////////////////////////////////////
// Canvas Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_canvas_view_t *g_canvas_model;

//////////////////////////////////////////////////////////////////////////////
// Hierarchy Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_hierarchy_view_t *g_hierarchy_scene;
extern ed_hierarchy_view_t *g_hierarchy_model;

//////////////////////////////////////////////////////////////////////////////
// Inspector Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_inspector_view_t *g_inspector_scene;
extern ed_inspector_view_t *g_inspector_model;

//////////////////////////////////////////////////////////////////////////////
// Profiler Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_profiler_view_t *g_profiler_scene;

//////////////////////////////////////////////////////////////////////////////
// Catalog Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_catalog_view_t *g_catalog_scene;
extern ed_catalog_view_t *g_catalog_model;

//////////////////////////////////////////////////////////////////////////////
// Dockspace Stuff
//////////////////////////////////////////////////////////////////////////////

extern uint8_t g_dockspace_is_dirty;

extern ed_dockspace_type_t g_dockspace_selected_type;

extern char const *g_dockspace_type_names[ED_DOCKSPACE_TYPE_COUNT];

//////////////////////////////////////////////////////////////////////////////
// Viewport Stuff
//////////////////////////////////////////////////////////////////////////////

extern ed_viewport_view_t *g_viewport_game;
extern ed_viewport_view_t *g_viewport_scene;
extern ed_viewport_view_t *g_viewport_model;

extern char const *g_viewport_gbuffer_attachment_names[ED_GBUFFER_ATTACHMENT_TYPE_COUNT];

#endif // ED_MAIN_H
