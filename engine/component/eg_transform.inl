__forceinline vector3_t eg_transform_local_right(eg_transform_t const *transform) {
  return quaternion_right(transform->local_rotation);
}
__forceinline vector3_t eg_transform_local_up(eg_transform_t const *transform) {
  return quaternion_up(transform->local_rotation);
}
__forceinline vector3_t eg_transform_local_front(eg_transform_t const *transform) {
  return quaternion_front(transform->local_rotation);
}
__forceinline vector3_t eg_transform_local_left(eg_transform_t const *transform) {
  return vector3_negate(quaternion_right(transform->local_rotation));
}
__forceinline vector3_t eg_transform_local_down(eg_transform_t const *transform) {
  return vector3_negate(quaternion_up(transform->local_rotation));
}
__forceinline vector3_t eg_transform_local_back(eg_transform_t const *transform) {
  return vector3_negate(quaternion_front(transform->local_rotation));
}

void eg_transform_set_position(eg_transform_t *transform, vector3_t position) {
  transform->local_position = position;
}
void eg_transform_set_position_xyz(eg_transform_t *transform, float x, float y, float z) {
  transform->local_position.x = x;
  transform->local_position.y = y;
  transform->local_position.z = z;
}
void eg_transform_set_relative_position(eg_transform_t *transform, eg_transform_t *reference, vector3_t position) {
  transform->local_position.x = reference->world_position.x + position.x;
  transform->local_position.y = reference->world_position.y + position.y;
  transform->local_position.z = reference->world_position.z + position.z;
}
void eg_transform_set_relative_position_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z) {
  transform->local_position.x = reference->world_position.x + x;
  transform->local_position.y = reference->world_position.y + y;
  transform->local_position.z = reference->world_position.z + z;
}

void eg_transform_set_rotation(eg_transform_t *transform, quaternion_t rotation) {
  transform->local_rotation = quaternion_norm(rotation);
}
void eg_transform_set_rotation_xyzw(eg_transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_norm(quaternion_xyzw(x, y, z, w));
}
void eg_transform_set_relative_rotation(eg_transform_t *transform, eg_transform_t *reference, quaternion_t rotation) {
  transform->local_rotation = quaternion_norm(quaternion_mul(reference->world_rotation, rotation));
}
void eg_transform_set_relative_rotation_xyzw(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_norm(quaternion_mul(reference->world_rotation, quaternion_xyzw(x, y, z, w)));
}

void eg_transform_set_euler_angles(eg_transform_t *transform, vector3_t euler_angles) {
  quaternion_t qx = quaternion_angle_axis(euler_angles.x, vector3_right());
  quaternion_t qy = quaternion_angle_axis(euler_angles.y, vector3_up());
  quaternion_t qz = quaternion_angle_axis(euler_angles.z, vector3_front());

  transform->local_rotation = quaternion_norm(quaternion_mul(qy, quaternion_mul(qx, qz)));
}
void eg_transform_set_euler_angles_pyr(eg_transform_t *transform, float p, float y, float r) {
  quaternion_t qx = quaternion_angle_axis(p, vector3_right());
  quaternion_t qy = quaternion_angle_axis(y, vector3_up());
  quaternion_t qz = quaternion_angle_axis(r, vector3_front());

  transform->local_rotation = quaternion_norm(quaternion_mul(qy, quaternion_mul(qx, qz)));
}
void eg_transform_set_relative_euler_angles(eg_transform_t *transform, eg_transform_t *reference, vector3_t euler_angles) {
  quaternion_t qx = quaternion_angle_axis(euler_angles.x, eg_transform_local_right(reference));
  quaternion_t qy = quaternion_angle_axis(euler_angles.y, eg_transform_local_up(reference));
  quaternion_t qz = quaternion_angle_axis(euler_angles.z, eg_transform_local_front(reference));

  transform->local_rotation = quaternion_norm(quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz))));
}
void eg_transform_set_relative_euler_angles_pyr(eg_transform_t *transform, eg_transform_t *reference, float p, float y, float r) {
  quaternion_t qx = quaternion_angle_axis(p, eg_transform_local_right(reference));
  quaternion_t qy = quaternion_angle_axis(y, eg_transform_local_up(reference));
  quaternion_t qz = quaternion_angle_axis(r, eg_transform_local_front(reference));

  transform->local_rotation = quaternion_norm(quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz))));
}

void eg_transform_set_scale(eg_transform_t *transform, vector3_t scale) {
  transform->local_scale = scale;
}
void eg_transform_set_scale_xyz(eg_transform_t *transform, float x, float y, float z) {
  transform->local_scale.x = x;
  transform->local_scale.y = y;
  transform->local_scale.z = z;
}
void eg_transform_set_relative_scale(eg_transform_t *transform, eg_transform_t *reference, vector3_t scale) {
  transform->local_scale.x = reference->world_scale.x * scale.x;
  transform->local_scale.y = reference->world_scale.y * scale.y;
  transform->local_scale.z = reference->world_scale.z * scale.z;
}
void eg_transform_set_relative_scale_xyz(eg_transform_t *transform, eg_transform_t *reference, float x, float y, float z) {
  transform->local_scale.x = reference->world_scale.x * x;
  transform->local_scale.y = reference->world_scale.y * y;
  transform->local_scale.z = reference->world_scale.z * z;
}
