#ifndef EG_SCENE_GAME_H
#define EG_SCENE_GAME_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

scene_t *game_create(context_t *context);
void game_update(scene_t *scene);
void game_destroy(scene_t *scene);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCENE_GAME_H
