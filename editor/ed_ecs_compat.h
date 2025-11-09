#ifndef ED_ECS_COMPAT_H
#define ED_ECS_COMPAT_H

// DISCLAIMER: This class exists solely to provide compatibility
//             between the ECS and the editor which is written in C++

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

camera_t const *ecs_camera(ecs_world_t const *world, ecs_entity_t entity);
camera_t *ecs_camera_mut(ecs_world_t const *world, ecs_entity_t entity);

editor_controller_t const *ecs_editor_controller(ecs_world_t const *world, ecs_entity_t entity);
editor_controller_t *ecs_editor_controller_mut(ecs_world_t const *world, ecs_entity_t entity);

rigidbody_t const *ecs_rigidbody(ecs_world_t const *world, ecs_entity_t entity);
rigidbody_t *ecs_rigidbody_mut(ecs_world_t const *world, ecs_entity_t entity);

transform_t const *ecs_transform(ecs_world_t const *world, ecs_entity_t entity);
transform_t *ecs_transform_mut(ecs_world_t const *world, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ED_ECS_COMPAT_H
