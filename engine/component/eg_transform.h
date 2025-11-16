#ifndef EG_TRANSFORM_COMPONENT_H
#define EG_TRANSFORM_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct eg_transform_t {
  vector3_t local_position;
  quaternion_t local_rotation;
  vector3_t local_scale;
  vector3_t world_position;
  quaternion_t world_rotation;
  vector3_t world_scale;
} eg_transform_t;

ECS_COMPONENT_DECLARE(eg_transform_t);

void eg_transform_init(eg_transform_t *transform);
matrix4_t eg_transform_matrix(eg_transform_t *transform);

void eg_transform_compute_world_position(ecs_world_t *world, ecs_entity_t entity);
void eg_transform_compute_world_rotation(ecs_world_t *world, ecs_entity_t entity);
void eg_transform_compute_world_scale(ecs_world_t *world, ecs_entity_t entity);

__forceinline vector3_t eg_transform_local_right(eg_transform_t const *transform);
__forceinline vector3_t eg_transform_local_up(eg_transform_t const *transform);
__forceinline vector3_t eg_transform_local_front(eg_transform_t const *transform);
__forceinline vector3_t eg_transform_local_left(eg_transform_t const *transform);
__forceinline vector3_t eg_transform_local_down(eg_transform_t const *transform);
__forceinline vector3_t eg_transform_local_back(eg_transform_t const *transform);

__forceinline void eg_transform_set_position(eg_transform_t *transform, vector3_t position);
__forceinline void eg_transform_set_position_xyz(eg_transform_t *transform, float x, float y, float z);
__forceinline void eg_transform_set_relative_position(eg_transform_t *transform, eg_transform_t *reference, vector3_t position);
__forceinline void eg_transform_set_relative_position_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z);

__forceinline void eg_transform_set_rotation(eg_transform_t *transform, quaternion_t rotation);
__forceinline void eg_transform_set_rotation_xyzw(eg_transform_t *transform, float x, float y, float z, float w);
__forceinline void eg_transform_set_relative_rotation(eg_transform_t *transform, eg_transform_t *reference, quaternion_t rotation);
__forceinline void eg_transform_set_relative_rotation_xyzw(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z, float w);

__forceinline void eg_transform_set_euler_angles(eg_transform_t *transform, vector3_t euler_angles);
__forceinline void eg_transform_set_euler_angles_pyr(eg_transform_t *transform, float p, float y, float r);
__forceinline void eg_transform_set_relative_euler_angles(eg_transform_t *transform, eg_transform_t *reference, vector3_t euler_angles);
__forceinline void eg_transform_set_relative_euler_angles_pyr(eg_transform_t *transform, eg_transform_t *reference, float p, float y, float r);

__forceinline void eg_transform_set_scale(eg_transform_t *transform, vector3_t scale);
__forceinline void eg_transform_set_scale_xyz(eg_transform_t *transform, float x, float y, float z);
__forceinline void eg_transform_set_relative_scale(eg_transform_t *transform, eg_transform_t *reference, vector3_t scale);
__forceinline void eg_transform_set_relative_scale_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/component/eg_transform.inl>

#endif // EG_TRANSFORM_COMPONENT_H
