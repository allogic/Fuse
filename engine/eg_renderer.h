#ifndef EG_RENDERER_H
#define EG_RENDERER_H

#define RENDERER_DEBUG_LINE_VERTEX_COUNT (1048576LL)
#define RENDERER_DEBUG_LINE_INDEX_COUNT (1048576LL)

#define RENDERER_MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceil((double)(GLOBAL_SIZE) / (LOCAL_SIZE)))

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;

typedef struct screen_info_t {
  vector2_t resolution;
} screen_info_t;

typedef struct camera_info_t {
  vector3_t world_position;
  int32_t reserved;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
  int32_t max_ray_steps;
} camera_info_t;

typedef struct debug_line_vertex_t {
  vector3_t position;
  vector4_t color;
} debug_line_vertex_t;

typedef uint32_t debug_line_index_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void renderer_create(void);
void renderer_update(void);
void renderer_draw(transform_t *transform, camera_t *camera);
void renderer_destroy(void);

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color);
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER_H
