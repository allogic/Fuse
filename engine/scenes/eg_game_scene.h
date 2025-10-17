#ifndef EG_GAME_SCENE_H
#define EG_GAME_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

scene_t *game_scene_create(void);
void game_scene_update(scene_t *scene);
void game_scene_destroy(scene_t *scene);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_GAME_SCENE_H
