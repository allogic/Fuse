#ifndef EG_CONTROLLER_COMPONENTS_H
#define EG_CONTROLLER_COMPONENTS_H

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
  float mouse_rotation_drag_damping;
} editor_controller_t;

ECS_COMPONENT_DECLARE(editor_controller_t);

void editor_controller_init(editor_controller_t *editor_controller);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTROLLER_COMPONENTS_H
