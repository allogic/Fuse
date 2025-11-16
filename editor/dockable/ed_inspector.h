#ifndef ED_INSPECTOR_H
#define ED_INSPECTOR_H

ed_inspector_t ed_inspector_create(eg_context_t *context);
void ed_inspector_refresh(ed_inspector_t *inspector);
void ed_inspector_draw(ed_inspector_t *inspector);
void ed_inspector_destroy(ed_inspector_t *inspector);

#endif // ED_INSPECTOR_H
