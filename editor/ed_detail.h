#ifndef ED_DETAIL_H
#define ED_DETAIL_H

extern int64_t g_detail_selected_model_mesh;
extern int64_t g_detail_selected_mesh_primitive;

extern vector_t g_detail_mesh_primitives;
extern vector_t g_detail_mesh_attributes;

void detail_create(context_t *context);
void detail_refresh(context_t *context);
void detail_draw(context_t *context);
void detail_destroy(context_t *context);

#endif // ED_DETAIL_H
