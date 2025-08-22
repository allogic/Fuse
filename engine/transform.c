#include <stdlib.h>
#include <string.h>

#include <library/core/heap.h>

#include <engine/transform.h>

transform_t *transform_create(transform_t *parent) {
  transform_t *transform = (transform_t *)core_heap_alloc(sizeof(transform_t));
  transform->parent = parent;
  transform->children = (transform_t **)core_heap_alloc(sizeof(transform_t *) * 16);
  transform->child_count = 0;
  transform->local_right = math_vector3_right();
  transform->local_up = math_vector3_up();
  transform->local_front = math_vector3_front();
  transform->local_left = math_vector3_left();
  transform->local_down = math_vector3_down();
  transform->local_back = math_vector3_back();
  transform->local_position = math_vector3_zero();
  transform->local_rotation = math_quaternion_identity();
  transform->local_scale = math_vector3_one();
  transform->world_position = math_vector3_zero();
  transform->world_rotation = math_quaternion_identity();
  transform->world_scale = math_vector3_one();

  if (parent) {
    parent->children[parent->child_count++] = transform;
  }

  return transform;
}
math_matrix4_t transform_matrix(transform_t *transform) {
  float xx = transform->world_rotation.x * transform->world_rotation.x;
  float yy = transform->world_rotation.y * transform->world_rotation.y;
  float zz = transform->world_rotation.z * transform->world_rotation.z;

  float xy = transform->world_rotation.x * transform->world_rotation.y;
  float xz = transform->world_rotation.x * transform->world_rotation.z;
  float yz = transform->world_rotation.y * transform->world_rotation.z;

  float wx = transform->world_rotation.w * transform->world_rotation.x;
  float wy = transform->world_rotation.w * transform->world_rotation.y;
  float wz = transform->world_rotation.w * transform->world_rotation.z;

  math_matrix4_t r = {
    (1.0F - 2.0F * (yy + zz)) * transform->world_scale.x,
    2.0F * (xy - wz) * transform->world_scale.y,
    2.0F * (xz + wy) * transform->world_scale.z,
    0.0F,
    2.0F * (xy + wz) * transform->world_scale.x,
    (1.0F - 2.0F * (xx + zz)) * transform->world_scale.y,
    2.0F * (yz - wx) * transform->world_scale.z,
    0.0F,
    2.0F * (xz - wy) * transform->world_scale.x,
    2.0F * (yz + wx) * transform->world_scale.y,
    (1.0F - 2.0F * (xx + yy)) * transform->world_scale.z,
    0.0F,
    transform->world_position.x,
    transform->world_position.y,
    transform->world_position.z,
    1.0F,
  };

  return r;
}
void transform_destroy(transform_t *transform) {
  core_heap_free(transform->children);
  core_heap_free(transform);
}

void transform_set_position(transform_t *transform, math_vector3_t position) {
  transform->local_position = position;

  transform_compute_world_position(transform);
}
void transform_set_position_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_position.x = x;
  transform->local_position.y = y;
  transform->local_position.z = z;

  transform_compute_world_position(transform);
}
void transform_set_relative_position(transform_t *transform, math_vector3_t position) {
  transform->local_position.x += position.x;
  transform->local_position.y += position.y;
  transform->local_position.z += position.z;

  transform_compute_world_position(transform);
}
void transform_set_relative_position_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_position.x += x;
  transform->local_position.y += y;
  transform->local_position.z += z;

  transform_compute_world_position(transform);
}
void transform_set_rotation(transform_t *transform, math_quaternion_t rotation) {
  transform->local_rotation = rotation;
  transform->local_rotation = math_quaternion_norm(transform->local_rotation);

  transform->local_right = math_quaternion_right(transform->local_rotation);
  transform->local_up = math_quaternion_up(transform->local_rotation);
  transform->local_front = math_quaternion_front(transform->local_rotation);

  transform->local_left = math_vector3_negate(transform->local_right);
  transform->local_down = math_vector3_negate(transform->local_up);
  transform->local_back = math_vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = math_quaternion_xyzw(x, y, z, w);
  transform->local_rotation = math_quaternion_norm(transform->local_rotation);

  transform->local_right = math_quaternion_right(transform->local_rotation);
  transform->local_up = math_quaternion_up(transform->local_rotation);
  transform->local_front = math_quaternion_front(transform->local_rotation);

  transform->local_left = math_vector3_negate(transform->local_right);
  transform->local_down = math_vector3_negate(transform->local_up);
  transform->local_back = math_vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_rotation(transform_t *transform, math_quaternion_t rotation) {
  transform->local_rotation = math_quaternion_mul(transform->local_rotation, rotation);
  transform->local_rotation = math_quaternion_norm(transform->local_rotation);

  transform->local_right = math_quaternion_right(transform->local_rotation);
  transform->local_up = math_quaternion_up(transform->local_rotation);
  transform->local_front = math_quaternion_front(transform->local_rotation);

  transform->local_left = math_vector3_negate(transform->local_right);
  transform->local_down = math_vector3_negate(transform->local_up);
  transform->local_back = math_vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = math_quaternion_mul(transform->local_rotation, math_quaternion_xyzw(x, y, z, w));
  transform->local_rotation = math_quaternion_norm(transform->local_rotation);

  transform->local_right = math_quaternion_right(transform->local_rotation);
  transform->local_up = math_quaternion_up(transform->local_rotation);
  transform->local_front = math_quaternion_front(transform->local_rotation);

  transform->local_left = math_vector3_negate(transform->local_right);
  transform->local_down = math_vector3_negate(transform->local_up);
  transform->local_back = math_vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_euler_angles(transform_t *transform, math_vector3_t rotation) {
  float p = math_utility_deg_to_rad(rotation.x);
  float y = math_utility_deg_to_rad(rotation.y);
  float r = math_utility_deg_to_rad(rotation.z);

  if (transform->parent) {
    math_quaternion_t qx = math_quaternion_angle_axis(p, transform->parent->local_right);
    math_quaternion_t qy = math_quaternion_angle_axis(y, transform->parent->local_up);
    math_quaternion_t qz = math_quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = math_quaternion_mul(qy, math_quaternion_mul(qx, qz));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  } else {
    math_quaternion_t qx = math_quaternion_angle_axis(p, math_vector3_right());
    math_quaternion_t qy = math_quaternion_angle_axis(y, math_vector3_up());
    math_quaternion_t qz = math_quaternion_angle_axis(r, math_vector3_front());

    transform->local_rotation = math_quaternion_mul(qy, math_quaternion_mul(qx, qz));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  p = math_utility_deg_to_rad(p);
  y = math_utility_deg_to_rad(y);
  r = math_utility_deg_to_rad(r);

  if (transform->parent) {
    math_quaternion_t qx = math_quaternion_angle_axis(p, transform->parent->local_right);
    math_quaternion_t qy = math_quaternion_angle_axis(y, transform->parent->local_up);
    math_quaternion_t qz = math_quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = math_quaternion_mul(qy, math_quaternion_mul(qx, qz));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  } else {
    math_quaternion_t qx = math_quaternion_angle_axis(p, math_vector3_right());
    math_quaternion_t qy = math_quaternion_angle_axis(y, math_vector3_up());
    math_quaternion_t qz = math_quaternion_angle_axis(r, math_vector3_front());

    transform->local_rotation = math_quaternion_mul(qy, math_quaternion_mul(qx, qz));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_euler_angles(transform_t *transform, math_vector3_t rotation) {
  float p = math_utility_deg_to_rad(rotation.x);
  float y = math_utility_deg_to_rad(rotation.y);
  float r = math_utility_deg_to_rad(rotation.z);

  if (transform->parent) {
    math_quaternion_t qx = math_quaternion_angle_axis(p, transform->parent->local_right);
    math_quaternion_t qy = math_quaternion_angle_axis(y, transform->parent->local_up);
    math_quaternion_t qz = math_quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = math_quaternion_mul(transform->local_rotation, math_quaternion_mul(qy, math_quaternion_mul(qx, qz)));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  } else {
    math_quaternion_t qx = math_quaternion_angle_axis(p, math_vector3_right());
    math_quaternion_t qy = math_quaternion_angle_axis(y, math_vector3_up());
    math_quaternion_t qz = math_quaternion_angle_axis(r, math_vector3_front());

    transform->local_rotation = math_quaternion_mul(transform->local_rotation, math_quaternion_mul(qy, math_quaternion_mul(qx, qz)));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  p = math_utility_deg_to_rad(p);
  y = math_utility_deg_to_rad(y);
  r = math_utility_deg_to_rad(r);

  if (transform->parent) {
    math_quaternion_t qx = math_quaternion_angle_axis(p, transform->parent->local_right);
    math_quaternion_t qy = math_quaternion_angle_axis(y, transform->parent->local_up);
    math_quaternion_t qz = math_quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = math_quaternion_mul(transform->local_rotation, math_quaternion_mul(qy, math_quaternion_mul(qx, qz)));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  } else {
    math_quaternion_t qx = math_quaternion_angle_axis(p, math_vector3_right());
    math_quaternion_t qy = math_quaternion_angle_axis(y, math_vector3_up());
    math_quaternion_t qz = math_quaternion_angle_axis(r, math_vector3_front());

    transform->local_rotation = math_quaternion_mul(transform->local_rotation, math_quaternion_mul(qy, math_quaternion_mul(qx, qz)));
    transform->local_rotation = math_quaternion_norm(transform->local_rotation);

    transform->local_right = math_quaternion_right(transform->local_rotation);
    transform->local_up = math_quaternion_up(transform->local_rotation);
    transform->local_front = math_quaternion_front(transform->local_rotation);

    transform->local_left = math_vector3_negate(transform->local_right);
    transform->local_down = math_vector3_negate(transform->local_up);
    transform->local_back = math_vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_scale(transform_t *transform, math_vector3_t scale) {
  transform->local_scale = scale;

  transform_compute_world_scale(transform);
}
void transform_set_scale_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_scale.x = x;
  transform->local_scale.y = y;
  transform->local_scale.z = z;

  transform_compute_world_scale(transform);
}
void transform_set_relative_scale(transform_t *transform, math_vector3_t scale) {
  transform->local_scale.x += scale.x;
  transform->local_scale.y += scale.y;
  transform->local_scale.z += scale.z;

  transform_compute_world_scale(transform);
}
void transform_set_relative_scale_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_scale.x += x;
  transform->local_scale.y += y;
  transform->local_scale.z += z;

  transform_compute_world_scale(transform);
}
