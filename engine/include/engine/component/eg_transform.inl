__forceinline eg_vector3_t eg_transform_local_right(eg_transform_t const *transform) {
  return eg_quaternion_right(transform->local_rotation);
}
__forceinline eg_vector3_t eg_transform_local_up(eg_transform_t const *transform) {
  return eg_quaternion_up(transform->local_rotation);
}
__forceinline eg_vector3_t eg_transform_local_front(eg_transform_t const *transform) {
  return eg_quaternion_front(transform->local_rotation);
}
__forceinline eg_vector3_t eg_transform_local_left(eg_transform_t const *transform) {
  return eg_vector3_negate(eg_quaternion_right(transform->local_rotation));
}
__forceinline eg_vector3_t eg_transform_local_down(eg_transform_t const *transform) {
  return eg_vector3_negate(eg_quaternion_up(transform->local_rotation));
}
__forceinline eg_vector3_t eg_transform_local_back(eg_transform_t const *transform) {
  return eg_vector3_negate(eg_quaternion_front(transform->local_rotation));
}

void eg_transform_set_position(eg_transform_t *transform, eg_vector3_t position) {
  transform->local_position = position;
}
void eg_transform_set_position_xyz(eg_transform_t *transform, float x, float y, float z) {
  transform->local_position.x = x;
  transform->local_position.y = y;
  transform->local_position.z = z;
}
void eg_transform_set_relative_position(eg_transform_t *transform, eg_transform_t *reference, eg_vector3_t position) {
  transform->local_position.x = reference->world_position.x + position.x;
  transform->local_position.y = reference->world_position.y + position.y;
  transform->local_position.z = reference->world_position.z + position.z;
}
void eg_transform_set_relative_position_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z) {
  transform->local_position.x = reference->world_position.x + x;
  transform->local_position.y = reference->world_position.y + y;
  transform->local_position.z = reference->world_position.z + z;
}

void eg_transform_set_rotation(eg_transform_t *transform, eg_quaternion_t rotation) {
  transform->local_rotation = eg_quaternion_norm(rotation);
}
void eg_transform_set_rotation_xyzw(eg_transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = eg_quaternion_norm(eg_quaternion_xyzw(x, y, z, w));
}
void eg_transform_set_relative_rotation(eg_transform_t *transform, eg_transform_t *reference, eg_quaternion_t rotation) {
  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(reference->world_rotation, rotation));
}
void eg_transform_set_relative_rotation_xyzw(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z, float w) {
  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(reference->world_rotation, eg_quaternion_xyzw(x, y, z, w)));
}

void eg_transform_set_euler_angles(eg_transform_t *transform, eg_vector3_t euler_angles) {
  eg_quaternion_t qx = eg_quaternion_angle_axis(euler_angles.x, eg_vector3_right());
  eg_quaternion_t qy = eg_quaternion_angle_axis(euler_angles.y, eg_vector3_up());
  eg_quaternion_t qz = eg_quaternion_angle_axis(euler_angles.z, eg_vector3_front());

  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(qy, eg_quaternion_mul(qx, qz)));
}
void eg_transform_set_euler_angles_pyr(eg_transform_t *transform, float p, float y, float r) {
  eg_quaternion_t qx = eg_quaternion_angle_axis(p, eg_vector3_right());
  eg_quaternion_t qy = eg_quaternion_angle_axis(y, eg_vector3_up());
  eg_quaternion_t qz = eg_quaternion_angle_axis(r, eg_vector3_front());

  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(qy, eg_quaternion_mul(qx, qz)));
}
void eg_transform_set_relative_euler_angles(eg_transform_t *transform, eg_transform_t *reference, eg_vector3_t euler_angles) {
  eg_quaternion_t qx = eg_quaternion_angle_axis(euler_angles.x, eg_transform_local_right(reference));
  eg_quaternion_t qy = eg_quaternion_angle_axis(euler_angles.y, eg_transform_local_up(reference));
  eg_quaternion_t qz = eg_quaternion_angle_axis(euler_angles.z, eg_transform_local_front(reference));

  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(transform->local_rotation, eg_quaternion_mul(qy, eg_quaternion_mul(qx, qz))));
}
void eg_transform_set_relative_euler_angles_pyr(eg_transform_t *transform, eg_transform_t *reference, float p, float y, float r) {
  eg_quaternion_t qx = eg_quaternion_angle_axis(p, eg_transform_local_right(reference));
  eg_quaternion_t qy = eg_quaternion_angle_axis(y, eg_transform_local_up(reference));
  eg_quaternion_t qz = eg_quaternion_angle_axis(r, eg_transform_local_front(reference));

  transform->local_rotation = eg_quaternion_norm(eg_quaternion_mul(transform->local_rotation, eg_quaternion_mul(qy, eg_quaternion_mul(qx, qz))));
}

void eg_transform_set_scale(eg_transform_t *transform, eg_vector3_t scale) {
  transform->local_scale = scale;
}
void eg_transform_set_scale_xyz(eg_transform_t *transform, float x, float y, float z) {
  transform->local_scale.x = x;
  transform->local_scale.y = y;
  transform->local_scale.z = z;
}
void eg_transform_set_relative_scale(eg_transform_t *transform, eg_transform_t *reference, eg_vector3_t scale) {
  transform->local_scale.x = reference->world_scale.x * scale.x;
  transform->local_scale.y = reference->world_scale.y * scale.y;
  transform->local_scale.z = reference->world_scale.z * scale.z;
}
void eg_transform_set_relative_scale_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z) {
  transform->local_scale.x = reference->world_scale.x * x;
  transform->local_scale.y = reference->world_scale.y * y;
  transform->local_scale.z = reference->world_scale.z * z;
}
