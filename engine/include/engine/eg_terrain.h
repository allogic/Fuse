#ifndef EG_TERRAIN_H
#define EG_TERRAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_terrain_t eg_terrain_create(eg_context_t *context);
void eg_terrain_rebuild(eg_terrain_t *terrain);
void eg_terrain_destroy(eg_terrain_t *terrain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_TERRAIN_H
