#ifndef ED_VIEWPORT_VIEW_H
#define ED_VIEWPORT_VIEW_H

ed_viewport_view_t *ed_viewport_view_create(eg_context_t *context);
void ed_viewport_view_refresh(ed_viewport_view_t *viewport);
void ed_viewport_view_draw(ed_viewport_view_t *viewport, uint8_t enable_controls);
void ed_viewport_view_destroy(ed_viewport_view_t *viewport);

#endif // ED_VIEWPORT_VIEW_H
