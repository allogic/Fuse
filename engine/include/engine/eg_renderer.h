#ifndef EG_RENDERER_H
#define EG_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_renderer_create(eg_context_t *context, lb_renderer_asset_id_t renderer_asset_id);
void eg_renderer_update(eg_renderer_t *renderer);
void eg_renderer_draw(eg_renderer_t *renderer);
void eg_renderer_destroy(eg_renderer_t *renderer);

uint8_t eg_renderer_is_dirty(eg_renderer_t *renderer);

void eg_renderer_set_dirty(eg_renderer_t *renderer, uint8_t is_dirty);

void eg_renderer_draw_debug_line(eg_renderer_t *renderer, lb_vector3_t from, lb_vector3_t to, lb_vector4_t color);
void eg_renderer_draw_debug_box(eg_renderer_t *renderer, lb_vector3_t position, lb_vector3_t size, lb_vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER_H
