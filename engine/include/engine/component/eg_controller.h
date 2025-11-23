#ifndef EG_CONTROLLER_COMPONENT_H
#define EG_CONTROLLER_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct eg_editor_controller_t {
  lb_vector3_t mouse_begin;
  lb_vector3_t mouse_end;
  lb_vector3_t mouse_delta;
  float keyboard_move_speed_fast;
  float keyboard_move_speed_normal;
  float mouse_move_speed_fast;
  float mouse_move_speed_normal;
  float mouse_rotation_speed;
  float mouse_rotation_drag_damping;
} eg_editor_controller_t;

ECS_COMPONENT_DECLARE(eg_editor_controller_t);

void eg_editor_controller_init(eg_editor_controller_t *editor_controller);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTROLLER_COMPONENT_H
