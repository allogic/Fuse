#include <stdlib.h>

#include <library/core/api.h>

#include <engine/context.h>
#include <engine/player.h>

#include <engine/renderer/api.h>

static void player_handle_position(player_t *player);
static void player_handle_rotation(player_t *player);
static void player_handle_raycast(player_t *player); // TODO

player_t *player_create(void) {
  player_t *player = (player_t *)core_heap_alloc(sizeof(player_t));
  player->camera = camera_create();
  player->transform = transform_create(0);
  player->velocity = math_vector3_zero();
  player->mouse_begin = math_vector3_zero();
  player->mouse_end = math_vector3_zero();
  player->mouse_delta = math_vector3_zero();
  player->keyboard_move_speed_fast = 50.0F;
  player->keyboard_move_speed_normal = 5.0F;
  player->mouse_move_speed_fast = 5.0F;
  player->mouse_move_speed_normal = 0.005F;
  player->mouse_rotation_speed = 0.1F;
  player->move_drag = 2.5F;
  player->rotation_drag = 50.0F;

  return player;
}
void player_update(player_t *player) {
  player_handle_position(player);
  player_handle_rotation(player);
  player_handle_raycast(player);
}
void player_destroy(player_t *player) {
  camera_destroy(player->camera);
  transform_destroy(player->transform);

  core_heap_free(player);
}

static void player_handle_position(player_t *player) {
  float keyboard_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                                ? player->keyboard_move_speed_fast
                                : player->keyboard_move_speed_normal;

  if (context_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    math_vector3_t right_direction = math_vector3_mul_scalar(player->transform->local_right, keyboard_move_speed);

    right_direction = math_vector3_mul_scalar(right_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, right_direction);
  }

  if (context_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    math_vector3_t left_direction = math_vector3_mul_scalar(player->transform->local_left, keyboard_move_speed);

    left_direction = math_vector3_mul_scalar(left_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, left_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_E)) {
    math_vector3_t up_direction = math_vector3_mul_scalar(player->transform->local_up, keyboard_move_speed);

    up_direction = math_vector3_mul_scalar(up_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, up_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    math_vector3_t down_direction = math_vector3_mul_scalar(player->transform->local_down, keyboard_move_speed);

    down_direction = math_vector3_mul_scalar(down_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, down_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    math_vector3_t front_direction = math_vector3_mul_scalar(player->transform->local_front, keyboard_move_speed);

    front_direction = math_vector3_mul_scalar(front_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, front_direction);
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    math_vector3_t back_direction = math_vector3_mul_scalar(player->transform->local_back, keyboard_move_speed);

    back_direction = math_vector3_mul_scalar(back_direction, (float)g_context_delta_time);

    transform_set_relative_position(player->transform, back_direction);
  }

  /* TODO
  vector3_t negative_velocity = scene_controller->velocity;

  negative_velocity.x = -negative_velocity.x;
  negative_velocity.y = -negative_velocity.y;
  negative_velocity.z = -negative_velocity.z;

  negative_velocity = math_vector3_mul_scalar(negative_velocity, scene_controller->move_drag);
  negative_velocity = math_vector3_mul_scalar(negative_velocity, g_delta_time);

  scene_controller->velocity = math_vector3_add(scene_controller->velocity, negative_velocity);

  vector3_t rotated_velocity = math_vector3_rotate(scene_controller->velocity, transform->local_rotation);

  transform_set_relative_position(transform, rotated_velocity);
  */
}
static void player_handle_rotation(player_t *player) {
  if (context_is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {
    player->mouse_begin = math_vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);
  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT) && context_is_mouse_key_held(MOUSE_KEY_LEFT)) {
    float mouse_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                               ? player->mouse_move_speed_fast
                               : player->mouse_move_speed_normal;

    player->mouse_end = math_vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);
    player->mouse_delta = math_vector3_sub(player->mouse_begin, player->mouse_end);

    float x_delta = player->mouse_delta.x * mouse_move_speed * (float)g_context_delta_time;
    float y_delta = -player->mouse_delta.y * mouse_move_speed * (float)g_context_delta_time;

    math_vector3_t change_in_x = math_vector3_mul_scalar(player->transform->local_right, x_delta);
    math_vector3_t change_in_y = math_vector3_mul_scalar(player->transform->local_up, y_delta);

    transform_set_relative_position(player->transform, change_in_x);
    transform_set_relative_position(player->transform, change_in_y);
  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT)) {
    player->mouse_end = math_vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);
    player->mouse_delta = math_vector3_sub(player->mouse_begin, player->mouse_end);

    float p_delta = -player->mouse_delta.y * player->mouse_rotation_speed * (float)g_context_delta_time;
    float y_delta = -player->mouse_delta.x * player->mouse_rotation_speed * (float)g_context_delta_time;

    math_quaternion_t local_rotation = transform_get_local_rotation(player->transform);

    math_quaternion_t qx = math_quaternion_angle_axis(p_delta, math_vector3_right());
    math_quaternion_t qy = math_quaternion_angle_axis(y_delta, math_vector3_up());

    local_rotation = math_quaternion_mul(qy, local_rotation);
    local_rotation = math_quaternion_mul(local_rotation, qx);

    transform_set_rotation(player->transform, local_rotation);
  }

  math_vector3_t mouse_damping = math_vector3_mul_scalar(player->mouse_delta, player->rotation_drag);

  mouse_damping = math_vector3_mul_scalar(mouse_damping, (float)g_context_delta_time);

  player->mouse_begin = math_vector3_sub(player->mouse_begin, mouse_damping);
}
static void player_handle_raycast(player_t *player) {
  {
    math_vector3_t position = {0.0F, 0.0F, 0.0F};
    math_vector3_t size = {32.0F, 32.0F, 32.0F};
    math_vector4_t color = {0.5F, 0.8F, 0.0F, 1.0F};

    renderer_draw_debug_box(position, size, color);
  }
}
