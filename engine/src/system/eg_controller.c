#include <engine/eg_pch.h>

#include <engine/system/eg_controller.h>

static void eg_editor_controller_handle_position(eg_editor_controller_t *editor_controller, eg_transform_t *transform, eg_rigidbody_t *rigidbody);
static void eg_editor_controller_handle_rotation(eg_editor_controller_t *editor_controller, eg_transform_t *transform, eg_rigidbody_t *rigidbody);

void eg_editor_controller_update(ecs_iter_t *iter) {
  while (ecs_query_next(iter)) {

    eg_transform_t *transforms = ecs_field(iter, eg_transform_t, 0);
    eg_rigidbody_t *rigidbodies = ecs_field(iter, eg_rigidbody_t, 1);
    eg_editor_controller_t *editor_controllers = ecs_field(iter, eg_editor_controller_t, 2);

    uint64_t entity_index = 0;
    uint64_t entity_count = iter->count;

    while (entity_index < entity_count) {

      eg_editor_controller_handle_position(&editor_controllers[entity_index], &transforms[entity_index], &rigidbodies[entity_index]);
      eg_editor_controller_handle_rotation(&editor_controllers[entity_index], &transforms[entity_index], &rigidbodies[entity_index]);

      entity_index++;
    }
  }
}

static void eg_editor_controller_handle_position(eg_editor_controller_t *editor_controller, eg_transform_t *transform, eg_rigidbody_t *rigidbody) {
  float keyboard_move_speed = eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_LEFT_SHIFT)
                                ? editor_controller->keyboard_move_speed_fast
                                : editor_controller->keyboard_move_speed_normal;

  eg_vector3_t velocity = rigidbody->linear_velocity;

  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_D)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_right(transform), keyboard_move_speed));
  }
  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_A)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_left(transform), keyboard_move_speed));
  }
  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_E)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_up(transform), keyboard_move_speed));
  }
  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_Q)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_down(transform), keyboard_move_speed));
  }
  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_W)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_front(transform), keyboard_move_speed));
  }
  if (eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_S)) {
    velocity = eg_vector3_add(velocity, eg_vector3_muls(eg_transform_local_back(transform), keyboard_move_speed));
  }

  rigidbody->linear_velocity = velocity;
}
static void eg_editor_controller_handle_rotation(eg_editor_controller_t *editor_controller, eg_transform_t *transform, eg_rigidbody_t *rigidbody) {
  float delta_time = eg_context_delta_time();
  uint32_t mouse_position_x = eg_context_mouse_position_x();
  uint32_t mouse_position_y = eg_context_mouse_position_y();

  if (eg_context_is_mouse_key_pressed(EG_MOUSE_KEY_RIGHT)) {

    editor_controller->mouse_begin = eg_vector3_xyz((float)mouse_position_x, (float)mouse_position_y, 0.0F);

  } else if (eg_context_is_mouse_key_held(EG_MOUSE_KEY_RIGHT) && eg_context_is_mouse_key_held(EG_MOUSE_KEY_LEFT)) {

    float mouse_move_speed = eg_context_is_keyboard_key_held(EG_KEYBOARD_KEY_LEFT_SHIFT)
                               ? editor_controller->mouse_move_speed_fast
                               : editor_controller->mouse_move_speed_normal;

    eg_vector3_t velocity = rigidbody->linear_velocity;

    editor_controller->mouse_end = eg_vector3_xyz((float)mouse_position_x, (float)mouse_position_y, 0.0F);
    editor_controller->mouse_delta = eg_vector3_sub(editor_controller->mouse_begin, editor_controller->mouse_end);

    float x_delta = editor_controller->mouse_delta.x * mouse_move_speed * delta_time;
    float y_delta = -editor_controller->mouse_delta.y * mouse_move_speed * delta_time;

    eg_vector3_t change_in_x = eg_vector3_muls(eg_transform_local_right(transform), x_delta);
    eg_vector3_t change_in_y = eg_vector3_muls(eg_transform_local_up(transform), y_delta);

    velocity = eg_vector3_add(eg_vector3_add(velocity, change_in_y), change_in_x);

    rigidbody->linear_velocity = velocity;

  } else if (eg_context_is_mouse_key_held(EG_MOUSE_KEY_RIGHT)) {

    eg_vector3_t velocity = rigidbody->angular_velocity;

    editor_controller->mouse_end = eg_vector3_xyz((float)mouse_position_x, (float)mouse_position_y, 0.0F);
    editor_controller->mouse_delta = eg_vector3_sub(editor_controller->mouse_begin, editor_controller->mouse_end);

    float p_delta = -editor_controller->mouse_delta.y * editor_controller->mouse_rotation_speed * delta_time;
    float y_delta = -editor_controller->mouse_delta.x * editor_controller->mouse_rotation_speed * delta_time;

    eg_vector3_t change_in_p = eg_vector3_muls(eg_vector3_right(), p_delta);
    eg_vector3_t change_in_y = eg_vector3_muls(eg_vector3_up(), y_delta);

    velocity = eg_vector3_add(eg_vector3_add(velocity, change_in_y), change_in_p);

    rigidbody->angular_velocity = velocity;
  }

  // TODO: How do I handle physics at one place..?

  eg_vector3_t mouse_damping = eg_vector3_muls(eg_vector3_muls(editor_controller->mouse_delta, editor_controller->mouse_rotation_drag_damping), delta_time);

  editor_controller->mouse_begin = eg_vector3_sub(editor_controller->mouse_begin, mouse_damping);
}
