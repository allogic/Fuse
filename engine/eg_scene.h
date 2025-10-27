#ifndef EG_SCENE_H
#define EG_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

scene_t *scene_create(context_t *context);
void scene_update(scene_t *scene);
void scene_destroy(scene_t *scene);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCENE_H
