#ifndef ED_CANVAS_H
#define ED_CANVAS_H

ed_canvas_t ed_canvas_create(eg_context_t *context);
void ed_canvas_refresh(ed_canvas_t *canvas);
void ed_canvas_draw(ed_canvas_t *canvas);
void ed_canvas_destroy(ed_canvas_t *canvas);

#endif // ED_CANVAS_H
