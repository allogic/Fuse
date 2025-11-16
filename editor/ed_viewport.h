#ifndef ED_VIEWPORT_H
#define ED_VIEWPORT_H

ed_viewport_t ed_viewport_create(eg_context_t *context);
void ed_viewport_refresh(ed_viewport_t *viewport);
void ed_viewport_draw(ed_viewport_t *viewport, uint8_t enable_controls);
void ed_viewport_destroy(ed_viewport_t *viewport);

#endif // ED_VIEWPORT_H
