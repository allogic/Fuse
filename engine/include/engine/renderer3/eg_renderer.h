#ifndef EG_RENDERER3_RENDERER_H
#define EG_RENDERER3_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_renderer_t *eg_renderer_current(void);

void eg_renderer_create(eg_renderer_asset_id_t renderer_asset_id);
void eg_renderer_update(void);
void eg_renderer_draw(void);
void eg_renderer_destroy(void);

uint8_t eg_renderer_is_dirty(void);
uint32_t eg_renderer_frames_in_flight(void);
uint32_t eg_renderer_frame_index(void);
uint32_t eg_renderer_image_index(void);
VkCommandBuffer *eg_renderer_command_buffer(void);

void eg_renderer_set_dirty(uint8_t is_dirty);

void eg_renderer_draw_debug_line(eg_vector3_t from, eg_vector3_t to, eg_vector4_t color);
void eg_renderer_draw_debug_box(eg_vector3_t position, eg_vector3_t size, eg_vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER3_RENDERER_H
