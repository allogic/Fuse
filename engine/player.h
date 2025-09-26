#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

#include <library/math/ma_api.h>

#include <engine/camera.h>
#include <engine/transform.h>

typedef struct player_t {
  camera_t *camera;
  transform_t *transform;
  vector3_t velocity;
  vector3_t mouse_begin;
  vector3_t mouse_end;
  vector3_t mouse_delta;
  float keyboard_move_speed_fast;
  float keyboard_move_speed_normal;
  float mouse_move_speed_fast;
  float mouse_move_speed_normal;
  float mouse_rotation_speed;
  float move_drag;
  float rotation_drag;
} player_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

player_t *player_create(void);
void player_update(player_t *player);
void player_destroy(player_t *player);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PLAYER_H
