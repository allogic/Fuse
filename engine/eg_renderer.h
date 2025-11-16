#ifndef EG_RENDERER_H
#define EG_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_renderer_create(eg_context_t *context);
void eg_renderer_update(eg_renderer_t *renderer);
void eg_renderer_draw(eg_renderer_t *renderer);
void eg_renderer_destroy(eg_renderer_t *renderer);

void eg_renderer_draw_debug_line(eg_renderer_t *renderer, vector3_t from, vector3_t to, vector4_t color);
void eg_renderer_draw_debug_box(eg_renderer_t *renderer, vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER_H
