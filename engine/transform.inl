__forceinline math_vector3_t transform_get_world_position(transform_t *transform) {
  return transform->world_position;
}
__forceinline math_quaternion_t transform_get_world_rotation(transform_t *transform) {
  return transform->world_rotation;
}
__forceinline math_vector3_t transform_get_world_euler_angles(transform_t *transform) {
  return math_quaternion_to_euler_angles(transform->world_rotation);
}
__forceinline math_vector3_t transform_get_world_scale(transform_t *transform) {
  return transform->world_scale;
}
__forceinline math_vector3_t transform_get_local_position(transform_t *transform) {
  return transform->local_position;
}
__forceinline math_quaternion_t transform_get_local_rotation(transform_t *transform) {
  return transform->local_rotation;
}
__forceinline math_vector3_t transform_get_local_euler_angles(transform_t *transform) {
  return math_quaternion_to_euler_angles(transform->local_rotation);
}
__forceinline math_vector3_t transform_get_local_scale(transform_t *transform) {
  return transform->local_scale;
}
__forceinline void transform_compute_world_position(transform_t *transform) {
  if (transform->parent) {
    transform->world_position = math_vector3_add(transform->parent->world_position, math_vector3_rotate(transform->local_position, transform->parent->world_rotation));
  } else {
    transform->world_position = transform->local_position;
  }

  uint64_t child_index = 0;
  while (child_index < transform->child_count) {
    transform_compute_world_position(transform->children[child_index]);

    child_index++;
  }
}
__forceinline void transform_compute_world_rotation(transform_t *transform) {
  if (transform->parent) {
    transform->world_rotation = math_quaternion_mul(transform->local_rotation, transform->parent->world_rotation);
  } else {
    transform->world_rotation = transform->local_rotation;
  }

  uint64_t child_index = 0;
  while (child_index < transform->child_count) {
    transform_compute_world_rotation(transform->children[child_index]);

    child_index++;
  }
}
__forceinline void transform_compute_world_scale(transform_t *transform) {
  if (transform->parent) {
    transform->world_scale = math_vector3_mul(transform->local_scale, transform->parent->world_scale);
  } else {
    transform->world_scale = transform->local_scale;
  }

  uint64_t child_index = 0;
  while (child_index < transform->child_count) {
    transform_compute_world_scale(transform->children[child_index]);

    child_index++;
  }
}
