#ifndef ED_DOCKSPACE_H
#define ED_DOCKSPACE_H

void dockspace_create(context_t *context);
void dockspace_refresh(context_t *context);
void dockspace_draw(context_t *context);
void dockspace_destroy(context_t *context);

uint8_t dockspace_begin_child(char const *name, uint8_t *is_open, uint8_t *is_docked);
void dockspace_end_child(uint8_t is_docked);

#endif // ED_DOCKSPACE_H
