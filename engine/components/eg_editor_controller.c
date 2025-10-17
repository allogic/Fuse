#include <engine/eg_pch.h>

#include <engine/components/eg_editor_controller.h>

static void editor_controller_handle_position(editor_controller_t *editor_controller);
static void editor_controller_handle_rotation(editor_controller_t *editor_controller);

editor_controller_t editor_controller_create(void) {
  editor_controller_t editor_controller = {0};

  editor_controller.mouse_begin = vector3_zero();
  editor_controller.mouse_end = vector3_zero();
  editor_controller.mouse_delta = vector3_zero();
  editor_controller.keyboard_move_speed_fast = 50.0F;
  editor_controller.keyboard_move_speed_normal = 5.0F;
  editor_controller.mouse_move_speed_fast = 5.0F;
  editor_controller.mouse_move_speed_normal = 0.005F;
  editor_controller.mouse_rotation_speed = 0.1F;
  editor_controller.move_drag = 2.5F;
  editor_controller.rotation_drag = 50.0F;

  return editor_controller;
}
void editor_controller_update(editor_controller_t *editor_controller) {
  editor_controller_handle_position(editor_controller);
  editor_controller_handle_rotation(editor_controller);
}

static void editor_controller_handle_position(editor_controller_t *editor_controller) {
  float keyboard_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                                ? player->keyboard_move_speed_fast
                                : player->keyboard_move_speed_normal;

  if (context_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    vector3_t right_direction = vector3_muls(player->transform->local_right, keyboard_move_speed);

    right_direction = vector3_muls(right_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, right_direction);
  }

  if (context_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    vector3_t left_direction = vector3_muls(player->transform->local_left, keyboard_move_speed);

    left_direction = vector3_muls(left_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, left_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_E)) {
    vector3_t up_direction = vector3_muls(player->transform->local_up, keyboard_move_speed);

    up_direction = vector3_muls(up_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, up_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    vector3_t down_direction = vector3_muls(player->transform->local_down, keyboard_move_speed);

    down_direction = vector3_muls(down_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, down_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    vector3_t front_direction = vector3_muls(player->transform->local_front, keyboard_move_speed);

    front_direction = vector3_muls(front_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, front_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    vector3_t back_direction = vector3_muls(player->transform->local_back, keyboard_move_speed);

    back_direction = vector3_muls(back_direction, (float)g_globals.context_delta_time);

    transform_set_relative_position(player->transform, back_direction);
  }

  /* TODO
  vector3_t negative_velocity = scene_controller->velocity;

  negative_velocity.x = -negative_velocity.x;
  negative_velocity.y = -negative_velocity.y;
  negative_velocity.z = -negative_velocity.z;

  negative_velocity = math_vector3_muls(negative_velocity, scene_controller->move_drag);
  negative_velocity = math_vector3_muls(negative_velocity, g_delta_time);

  scene_controller->velocity = math_vector3_add(scene_controller->velocity, negative_velocity);

  vector3_t rotated_velocity = math_vector3_rotate(scene_controller->velocity, transform->local_rotation);

  transform_set_relative_position(transform, rotated_velocity);
  */
}
static void editor_controller_handle_rotation(editor_controller_t *editor_controller) {
  if (context_is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {
    player->mouse_begin = vector3_xyz((float)g_globals.context_mouse_position_x, (float)g_globals.context_mouse_position_y, 0.0F);
  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT) && context_is_mouse_key_held(MOUSE_KEY_LEFT)) {
    float mouse_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                               ? player->mouse_move_speed_fast
                               : player->mouse_move_speed_normal;

    player->mouse_end = vector3_xyz((float)g_globals.context_mouse_position_x, (float)g_globals.context_mouse_position_y, 0.0F);
    player->mouse_delta = vector3_sub(player->mouse_begin, player->mouse_end);

    float x_delta = player->mouse_delta.x * mouse_move_speed * (float)g_globals.context_delta_time;
    float y_delta = -player->mouse_delta.y * mouse_move_speed * (float)g_globals.context_delta_time;

    vector3_t change_in_x = vector3_muls(player->transform->local_right, x_delta);
    vector3_t change_in_y = vector3_muls(player->transform->local_up, y_delta);

    transform_set_relative_position(player->transform, change_in_x);
    transform_set_relative_position(player->transform, change_in_y);
  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT)) {
    player->mouse_end = vector3_xyz((float)g_globals.context_mouse_position_x, (float)g_globals.context_mouse_position_y, 0.0F);
    player->mouse_delta = vector3_sub(player->mouse_begin, player->mouse_end);

    float p_delta = -player->mouse_delta.y * player->mouse_rotation_speed * (float)g_globals.context_delta_time;
    float y_delta = -player->mouse_delta.x * player->mouse_rotation_speed * (float)g_globals.context_delta_time;

    quaternion_t local_rotation = transform_get_local_rotation(player->transform);

    quaternion_t qx = quaternion_angle_axis(p_delta, vector3_right());
    quaternion_t qy = quaternion_angle_axis(y_delta, vector3_up());

    local_rotation = quaternion_mul(qy, local_rotation);
    local_rotation = quaternion_mul(local_rotation, qx);

    transform_set_rotation(player->transform, local_rotation);
  }

  vector3_t mouse_damping = vector3_muls(player->mouse_delta, player->rotation_drag);

  mouse_damping = vector3_muls(mouse_damping, (float)g_globals.context_delta_time);

  player->mouse_begin = vector3_sub(player->mouse_begin, mouse_damping);
}
