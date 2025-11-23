#ifndef ED_DOCKSPACE_H
#define ED_DOCKSPACE_H

void ed_dockspace_refresh(eg_context_t *context);
void ed_dockspace_draw(eg_context_t *context);

uint8_t ed_dockspace_begin_child(char const *name, uint8_t *is_open, uint8_t *is_docked);
void ed_dockspace_end_child(uint8_t is_docked);

#endif // ED_DOCKSPACE_H
