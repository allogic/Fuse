#ifndef EG_PLAYER_H
#define EG_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

player_t *player_create(void);
void player_update(player_t *player);
void player_destroy(player_t *player);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PLAYER_H
