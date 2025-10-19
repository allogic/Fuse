#ifndef EG_RENDERER_H
#define EG_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void renderer_create(void);
void renderer_update(void);
void renderer_draw(void);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER_H
