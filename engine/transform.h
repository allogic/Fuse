#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdint.h>

#include <library/math/ma_api.h>

typedef struct transform_t {
  struct transform_t *parent;
  struct transform_t **children;
  uint64_t child_count;
  vector3_t local_right;
  vector3_t local_up;
  vector3_t local_front;
  vector3_t local_left;
  vector3_t local_down;
  vector3_t local_back;
  vector3_t local_position;
  quaternion_t local_rotation;
  vector3_t local_scale;
  vector3_t world_position;
  quaternion_t world_rotation;
  vector3_t world_scale;
} transform_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

transform_t *transform_create(transform_t *parent);
matrix4_t transform_matrix(transform_t *transform);
void transform_destroy(transform_t *transform);

__forceinline vector3_t transform_get_world_position(transform_t *transform);
__forceinline quaternion_t transform_get_world_rotation(transform_t *transform);
__forceinline vector3_t transform_get_world_euler_angles(transform_t *transform);
__forceinline vector3_t transform_get_world_scale(transform_t *transform);

__forceinline vector3_t transform_get_local_position(transform_t *transform);
__forceinline quaternion_t transform_get_local_rotation(transform_t *transform);
__forceinline vector3_t transform_get_local_euler_angles(transform_t *transform);
__forceinline vector3_t transform_get_local_scale(transform_t *transform);

void transform_set_position(transform_t *transform, vector3_t position);
void transform_set_position_xyz(transform_t *transform, float x, float y, float z);
void transform_set_relative_position(transform_t *transform, vector3_t position);
void transform_set_relative_position_xyz(transform_t *transform, float x, float y, float z);
void transform_set_rotation(transform_t *transform, quaternion_t rotation);
void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w);
void transform_set_relative_rotation(transform_t *transform, quaternion_t rotation);
void transform_set_relative_rotation_xyzw(transform_t *transform, float x, float y, float z, float w);
void transform_set_euler_angles(transform_t *transform, vector3_t rotation);
void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r);
void transform_set_relative_euler_angles(transform_t *transform, vector3_t rotation);
void transform_set_relative_euler_angles_pyr(transform_t *transform, float p, float y, float r);
void transform_set_scale(transform_t *transform, vector3_t scale);
void transform_set_scale_xyz(transform_t *transform, float x, float y, float z);
void transform_set_relative_scale(transform_t *transform, vector3_t scale);
void transform_set_relative_scale_xyz(transform_t *transform, float x, float y, float z);

__forceinline void transform_compute_world_position(transform_t *transform);
__forceinline void transform_compute_world_rotation(transform_t *transform);
__forceinline void transform_compute_world_scale(transform_t *transform);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/transform.inl>

#endif // TRANSFORM_H
