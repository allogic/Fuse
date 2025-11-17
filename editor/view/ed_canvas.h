#ifndef ED_CANVAS_VIEW_H
#define ED_CANVAS_VIEW_H

ed_canvas_view_t *ed_canvas_view_create(eg_context_t *context);
void ed_canvas_view_draw(ed_canvas_view_t *canvas);
void ed_canvas_view_destroy(ed_canvas_view_t *canvas);

#endif // ED_CANVAS_VIEW_H
