#ifndef EG_SCENE_MANAGER_H
#define EG_SCENE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void scene_manager_create(void);
scene_t *scene_manager_active_scene(void);
void scene_manager_set_active_scene(scene_t *scene);
void scene_manager_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCENE_MANAGER_H
