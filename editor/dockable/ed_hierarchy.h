#ifndef ED_HIERARCHY_H
#define ED_HIERARCHY_H

ed_hierarchy_t ed_hierarchy_create(eg_context_t *context);
void ed_hierarchy_refresh(ed_hierarchy_t *hierarchy);
void ed_hierarchy_draw(ed_hierarchy_t *hierarchy);
void ed_hierarchy_destroy(ed_hierarchy_t *hierarchy);

#endif // ED_HIERARCHY_H
