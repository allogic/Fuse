#ifndef ED_DOCKSPACE_H
#define ED_DOCKSPACE_H

extern bool dockspace_layout_is_dirty;

void dockspace_create();
void dockspace_refresh();
void dockspace_draw(context_t *context);
void dockspace_destroy();

#endif // ED_DOCKSPACE_H
