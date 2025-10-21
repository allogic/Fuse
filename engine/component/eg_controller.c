#include <engine/eg_pch.h>

#include <engine/component/eg_controller.h>

void editor_controller_init(editor_controller_t *editor_controller) {
  editor_controller->mouse_begin = vector3_zero();
  editor_controller->mouse_end = vector3_zero();
  editor_controller->mouse_delta = vector3_zero();
  editor_controller->keyboard_move_speed_fast = 0.01F;
  editor_controller->keyboard_move_speed_normal = 0.001F;
  editor_controller->mouse_move_speed_fast = 0.01F;
  editor_controller->mouse_move_speed_normal = 0.001F;
  editor_controller->mouse_rotation_speed = 0.01F;
  editor_controller->mouse_rotation_drag_damping = 50.0F;
}
