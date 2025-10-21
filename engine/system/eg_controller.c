#include <engine/eg_pch.h>
#include <engine/eg_context.h> // TODO: move event stuff to some kind of global header..

#include <engine/system/eg_controller.h>

static void editor_controller_handle_position(editor_controller_t *editor_controller, transform_t *transform, rigidbody_t *rigidbody);
static void editor_controller_handle_rotation(editor_controller_t *editor_controller, transform_t *transform, rigidbody_t *rigidbody);

void editor_controller_update(ecs_iter_t *iter) {
  while (ecs_query_next(iter)) {

    transform_t *transforms = ecs_field(iter, transform_t, 0);
    rigidbody_t *rigidbodies = ecs_field(iter, rigidbody_t, 1);
    editor_controller_t *editor_controllers = ecs_field(iter, editor_controller_t, 2);

    uint64_t entity_index = 0;
    uint64_t entity_count = iter->count;

    while (entity_index < entity_count) {

      editor_controller_handle_position(&editor_controllers[entity_index], &transforms[entity_index], &rigidbodies[entity_index]);
      editor_controller_handle_rotation(&editor_controllers[entity_index], &transforms[entity_index], &rigidbodies[entity_index]);

      entity_index++;
    }
  }
}

static void editor_controller_handle_position(editor_controller_t *editor_controller, transform_t *transform, rigidbody_t *rigidbody) {
  float keyboard_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                                ? editor_controller->keyboard_move_speed_fast
                                : editor_controller->keyboard_move_speed_normal;

  vector3_t velocity = rigidbody->linear_velocity;

  if (context_is_keyboard_key_held(KEYBOARD_KEY_D)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_right(transform), keyboard_move_speed));
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_A)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_left(transform), keyboard_move_speed));
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_E)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_up(transform), keyboard_move_speed));
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_Q)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_down(transform), keyboard_move_speed));
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_W)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_front(transform), keyboard_move_speed));
  }
  if (context_is_keyboard_key_held(KEYBOARD_KEY_S)) {
    velocity = vector3_add(velocity, vector3_muls(transform_local_back(transform), keyboard_move_speed));
  }

  rigidbody->linear_velocity = velocity;
}
static void editor_controller_handle_rotation(editor_controller_t *editor_controller, transform_t *transform, rigidbody_t *rigidbody) {
  if (context_is_mouse_key_pressed(MOUSE_KEY_RIGHT)) {

    editor_controller->mouse_begin = vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);

  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT) && context_is_mouse_key_held(MOUSE_KEY_LEFT)) {

    float mouse_move_speed = context_is_keyboard_key_held(KEYBOARD_KEY_LEFT_SHIFT)
                               ? editor_controller->mouse_move_speed_fast
                               : editor_controller->mouse_move_speed_normal;

    vector3_t velocity = rigidbody->linear_velocity;

    editor_controller->mouse_end = vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);
    editor_controller->mouse_delta = vector3_sub(editor_controller->mouse_begin, editor_controller->mouse_end);

    float x_delta = editor_controller->mouse_delta.x * mouse_move_speed * (float)g_context_delta_time;
    float y_delta = -editor_controller->mouse_delta.y * mouse_move_speed * (float)g_context_delta_time;

    vector3_t change_in_x = vector3_muls(transform_local_right(transform), x_delta);
    vector3_t change_in_y = vector3_muls(transform_local_up(transform), y_delta);

    velocity = vector3_add(vector3_add(velocity, change_in_y), change_in_x);

    rigidbody->linear_velocity = velocity;

  } else if (context_is_mouse_key_held(MOUSE_KEY_RIGHT)) {

    vector3_t velocity = rigidbody->angular_velocity;

    editor_controller->mouse_end = vector3_xyz((float)g_context_mouse_position_x, (float)g_context_mouse_position_y, 0.0F);
    editor_controller->mouse_delta = vector3_sub(editor_controller->mouse_begin, editor_controller->mouse_end);

    float p_delta = -editor_controller->mouse_delta.y * editor_controller->mouse_rotation_speed * (float)g_context_delta_time;
    float y_delta = -editor_controller->mouse_delta.x * editor_controller->mouse_rotation_speed * (float)g_context_delta_time;

    vector3_t change_in_p = vector3_muls(vector3_right(), p_delta);
    vector3_t change_in_y = vector3_muls(vector3_up(), y_delta);

    velocity = vector3_add(vector3_add(velocity, change_in_y), change_in_p);

    rigidbody->angular_velocity = velocity;
  }

  // TODO: How do I handle physics at one place..?

  vector3_t mouse_damping = vector3_muls(vector3_muls(editor_controller->mouse_delta, editor_controller->mouse_rotation_drag_damping), (float)g_context_delta_time);

  editor_controller->mouse_begin = vector3_sub(editor_controller->mouse_begin, mouse_damping);
}
