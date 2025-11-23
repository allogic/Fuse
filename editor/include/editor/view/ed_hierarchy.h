#ifndef ED_HIERARCHY_VIEW_H
#define ED_HIERARCHY_VIEW_H

ed_hierarchy_view_t *ed_hierarchy_view_create(eg_context_t *context);
void ed_hierarchy_view_refresh(ed_hierarchy_view_t *hierarchy);
void ed_hierarchy_view_draw(ed_hierarchy_view_t *hierarchy);
void ed_hierarchy_view_destroy(ed_hierarchy_view_t *hierarchy);

#endif // ED_HIERARCHY_VIEW_H
