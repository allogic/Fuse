#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <engine/transform.h>
#include <engine/camera.h>

#include <engine/math/api.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t g_renderer_is_dirty;

extern uint8_t g_renderer_enable_debug;

extern int32_t g_renderer_frames_in_flight;

extern int32_t g_renderer_chunk_count_x;
extern int32_t g_renderer_chunk_count_y;
extern int32_t g_renderer_chunk_count_z;

extern int32_t g_renderer_chunk_size;

void renderer_create(void);
void renderer_update(void);
void renderer_draw(transform_t *transform, camera_t *camera);
void renderer_destroy(void);

void renderer_draw_debug_line(math_vector3_t from, math_vector3_t to, math_vector4_t color);
void renderer_draw_debug_box(math_vector3_t position, math_vector3_t size, math_vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RENDERER_H
