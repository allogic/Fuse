#ifndef EG_SCENE_H
#define EG_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

scene_t *scene_create(context_t *context);
void scene_update(scene_t *scene);
void scene_destroy(scene_t *scene);

camera_t const *scene_camera(scene_t *scene, ecs_entity_t entity);
camera_t *scene_camera_mut(scene_t *scene, ecs_entity_t entity);

editor_controller_t const *scene_editor_controller(scene_t *scene, ecs_entity_t entity);
editor_controller_t *scene_editor_controller_mut(scene_t *scene, ecs_entity_t entity);

rigidbody_t const *scene_rigidbody(scene_t *scene, ecs_entity_t entity);
rigidbody_t *scene_rigidbody_mut(scene_t *scene, ecs_entity_t entity);

transform_t const *scene_transform(scene_t *scene, ecs_entity_t entity);
transform_t *scene_transform_mut(scene_t *scene, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCENE_H
