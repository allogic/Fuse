#ifndef ED_DOCKSPACE_H
#define ED_DOCKSPACE_H

void dockspace_create(context_t *context);
void dockspace_refresh(context_t *context);
void dockspace_draw(context_t *context);
void dockspace_destroy(context_t *context);

bool dockspace_begin_child(char const *name, bool *is_open, bool *is_docked);
void dockspace_end_child(bool is_docked);

#endif // ED_DOCKSPACE_H
