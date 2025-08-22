#ifndef RENDERER_FORWARD_H
#define RENDERER_FORWARD_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#include <library/math/api.h>

typedef struct renderer_cluster_vertex_t {
  math_ivector3_t position;
} renderer_cluster_vertex_t;

typedef uint32_t renderer_cluster_index_t;

typedef struct renderer_debug_line_vertex_t {
  math_vector3_t position;
  math_vector4_t color;
} renderer_debug_line_vertex_t;

typedef uint32_t renderer_debug_line_index_t;

typedef struct renderer_time_t {
  float time;
  float delta_time;
} renderer_time_t;

typedef struct renderer_screen_t {
  math_vector2_t resolution;
} renderer_screen_t;

typedef struct renderer_camera_t {
  math_vector3_t world_position;
  int32_t reserved;
  math_matrix4_t view;
  math_matrix4_t projection;
  math_matrix4_t view_projection;
  math_matrix4_t view_projection_inv;
  int32_t max_ray_steps;
} renderer_camera_t;

typedef struct renderer_cluster_t {
  math_ivector3_t chunk_count;
  int32_t chunk_size;
  int32_t max_lod_levels;
} renderer_cluster_t;

typedef struct renderer_chunk_editor_push_constant_t {
  math_ivector3_t world_position;
  int32_t lod;
} renderer_chunk_editor_push_constant_t;

typedef struct renderer_chunk_generator_push_constant_t {
  math_ivector3_t world_position;
  int32_t lod;
} renderer_chunk_generator_push_constant_t;

typedef struct renderer_chunk_mipmap_push_constant_t {
  math_ivector3_t world_position;
  int32_t lod;
} renderer_chunk_mipmap_push_constant_t;

#endif // RENDERER_FORWARD_H
