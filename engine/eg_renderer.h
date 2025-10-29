#ifndef EG_RENDERER_H
#define EG_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void renderer_create(context_t *context);
void renderer_update(renderer_t *renderer);
void renderer_draw(renderer_t *renderer);
void renderer_destroy(renderer_t *renderer);

void renderer_draw_debug_line(renderer_t *renderer, vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(renderer_t *renderer, vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER_H
