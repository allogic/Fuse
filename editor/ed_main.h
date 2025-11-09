#ifndef ED_MAIN_H
#define ED_MAIN_H

#define EDITOR_BACKGROUND_COLOR IM_COL32(25, 25, 25, 255)
#define EDITOR_DOCKING_BACKGROUND_COLOR IM_COL32(50, 50, 50, 255)
#define EDITOR_HIGHLIGHT_COLOR IM_COL32(70, 70, 70, 255)
#define EDITOR_ACTIVE_COLOR IM_COL32(90, 90, 90, 255)

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

extern vector_t g_editor_scenes;

extern int64_t g_editor_selected_scene_asset;

extern sceneview_t *g_editor_sceneview[0xFF];

#endif // ED_MAIN_H
