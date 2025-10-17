#ifndef EG_EDITOR_CONTROLLER_H
#define EG_EDITOR_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct editor_controller_t {
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
} editor_controller_t;

ECS_COMPONENT_DECLARE(editor_controller_t);

editor_controller_t editor_controller_create(void);
void editor_controller_update(editor_controller_t *editor_controller);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_EDITOR_CONTROLLER_H
