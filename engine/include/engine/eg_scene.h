#ifndef EG_SCENE_H
#define EG_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_scene_t *eg_scene_create(eg_context_t *context, lb_scene_asset_id_t scene_asset_id);
void eg_scene_update(eg_scene_t *scene);
void eg_scene_destroy(eg_scene_t *scene);

ecs_world_t *eg_scene_world(eg_scene_t *scene);
ecs_entity_t eg_scene_root(eg_scene_t *scene);
ecs_entity_t eg_scene_player(eg_scene_t *scene);

eg_camera_t const *eg_scene_camera(eg_scene_t *scene, ecs_entity_t entity);
eg_camera_t *eg_scene_camera_mut(eg_scene_t *scene, ecs_entity_t entity);

eg_editor_controller_t const *eg_scene_editor_controller(eg_scene_t *scene, ecs_entity_t entity);
eg_editor_controller_t *eg_scene_editor_controller_mut(eg_scene_t *scene, ecs_entity_t entity);

eg_rigidbody_t const *eg_scene_rigidbody(eg_scene_t *scene, ecs_entity_t entity);
eg_rigidbody_t *eg_scene_rigidbody_mut(eg_scene_t *scene, ecs_entity_t entity);

eg_script_t const *eg_scene_script(eg_scene_t *scene, ecs_entity_t entity);
eg_script_t *eg_scene_script_mut(eg_scene_t *scene, ecs_entity_t entity);

eg_transform_t const *eg_scene_transform(eg_scene_t *scene, ecs_entity_t entity);
eg_transform_t *eg_scene_transform_mut(eg_scene_t *scene, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCENE_H
