#include <stdlib.h>
#include <string.h>

#include <library/core/heap.h>

#include <engine/transform.h>

transform_t *transform_create(transform_t *parent) {
  transform_t *transform = (transform_t *)heap_alloc(sizeof(transform_t));
  transform->parent = parent;
  transform->children = (transform_t **)heap_alloc(sizeof(transform_t *) * 16);
  transform->child_count = 0;
  transform->local_right = vector3_right();
  transform->local_up = vector3_up();
  transform->local_front = vector3_front();
  transform->local_left = vector3_left();
  transform->local_down = vector3_down();
  transform->local_back = vector3_back();
  transform->local_position = vector3_zero();
  transform->local_rotation = quaternion_identity();
  transform->local_scale = vector3_one();
  transform->world_position = vector3_zero();
  transform->world_rotation = quaternion_identity();
  transform->world_scale = vector3_one();

  if (parent) {
    parent->children[parent->child_count++] = transform;
  }

  return transform;
}
matrix4_t transform_matrix(transform_t *transform) {
  float xx = transform->world_rotation.x * transform->world_rotation.x;
  float yy = transform->world_rotation.y * transform->world_rotation.y;
  float zz = transform->world_rotation.z * transform->world_rotation.z;

  float xy = transform->world_rotation.x * transform->world_rotation.y;
  float xz = transform->world_rotation.x * transform->world_rotation.z;
  float yz = transform->world_rotation.y * transform->world_rotation.z;

  float wx = transform->world_rotation.w * transform->world_rotation.x;
  float wy = transform->world_rotation.w * transform->world_rotation.y;
  float wz = transform->world_rotation.w * transform->world_rotation.z;

  matrix4_t r = {
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
  heap_free(transform->children);
  heap_free(transform);
}

void transform_set_position(transform_t *transform, vector3_t position) {
  transform->local_position = position;

  transform_compute_world_position(transform);
}
void transform_set_position_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_position.x = x;
  transform->local_position.y = y;
  transform->local_position.z = z;

  transform_compute_world_position(transform);
}
void transform_set_relative_position(transform_t *transform, vector3_t position) {
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
void transform_set_rotation(transform_t *transform, quaternion_t rotation) {
  transform->local_rotation = rotation;
  transform->local_rotation = quaternion_norm(transform->local_rotation);

  transform->local_right = quaternion_right(transform->local_rotation);
  transform->local_up = quaternion_up(transform->local_rotation);
  transform->local_front = quaternion_front(transform->local_rotation);

  transform->local_left = vector3_negate(transform->local_right);
  transform->local_down = vector3_negate(transform->local_up);
  transform->local_back = vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_xyzw(x, y, z, w);
  transform->local_rotation = quaternion_norm(transform->local_rotation);

  transform->local_right = quaternion_right(transform->local_rotation);
  transform->local_up = quaternion_up(transform->local_rotation);
  transform->local_front = quaternion_front(transform->local_rotation);

  transform->local_left = vector3_negate(transform->local_right);
  transform->local_down = vector3_negate(transform->local_up);
  transform->local_back = vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_rotation(transform_t *transform, quaternion_t rotation) {
  transform->local_rotation = quaternion_mul(transform->local_rotation, rotation);
  transform->local_rotation = quaternion_norm(transform->local_rotation);

  transform->local_right = quaternion_right(transform->local_rotation);
  transform->local_up = quaternion_up(transform->local_rotation);
  transform->local_front = quaternion_front(transform->local_rotation);

  transform->local_left = vector3_negate(transform->local_right);
  transform->local_down = vector3_negate(transform->local_up);
  transform->local_back = vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_rotation_xyzw(transform_t *transform, float x, float y, float z, float w) {
  transform->local_rotation = quaternion_mul(transform->local_rotation, quaternion_xyzw(x, y, z, w));
  transform->local_rotation = quaternion_norm(transform->local_rotation);

  transform->local_right = quaternion_right(transform->local_rotation);
  transform->local_up = quaternion_up(transform->local_rotation);
  transform->local_front = quaternion_front(transform->local_rotation);

  transform->local_left = vector3_negate(transform->local_right);
  transform->local_down = vector3_negate(transform->local_up);
  transform->local_back = vector3_negate(transform->local_front);

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_euler_angles(transform_t *transform, vector3_t rotation) {
  float p = math_deg_to_rad(rotation.x);
  float y = math_deg_to_rad(rotation.y);
  float r = math_deg_to_rad(rotation.z);

  if (transform->parent) {
    quaternion_t qx = quaternion_angle_axis(p, transform->parent->local_right);
    quaternion_t qy = quaternion_angle_axis(y, transform->parent->local_up);
    quaternion_t qz = quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = quaternion_mul(qy, quaternion_mul(qx, qz));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  } else {
    quaternion_t qx = quaternion_angle_axis(p, vector3_right());
    quaternion_t qy = quaternion_angle_axis(y, vector3_up());
    quaternion_t qz = quaternion_angle_axis(r, vector3_front());

    transform->local_rotation = quaternion_mul(qy, quaternion_mul(qx, qz));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  p = math_deg_to_rad(p);
  y = math_deg_to_rad(y);
  r = math_deg_to_rad(r);

  if (transform->parent) {
    quaternion_t qx = quaternion_angle_axis(p, transform->parent->local_right);
    quaternion_t qy = quaternion_angle_axis(y, transform->parent->local_up);
    quaternion_t qz = quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = quaternion_mul(qy, quaternion_mul(qx, qz));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  } else {
    quaternion_t qx = quaternion_angle_axis(p, vector3_right());
    quaternion_t qy = quaternion_angle_axis(y, vector3_up());
    quaternion_t qz = quaternion_angle_axis(r, vector3_front());

    transform->local_rotation = quaternion_mul(qy, quaternion_mul(qx, qz));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_euler_angles(transform_t *transform, vector3_t rotation) {
  float p = math_deg_to_rad(rotation.x);
  float y = math_deg_to_rad(rotation.y);
  float r = math_deg_to_rad(rotation.z);

  if (transform->parent) {
    quaternion_t qx = quaternion_angle_axis(p, transform->parent->local_right);
    quaternion_t qy = quaternion_angle_axis(y, transform->parent->local_up);
    quaternion_t qz = quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz)));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  } else {
    quaternion_t qx = quaternion_angle_axis(p, vector3_right());
    quaternion_t qy = quaternion_angle_axis(y, vector3_up());
    quaternion_t qz = quaternion_angle_axis(r, vector3_front());

    transform->local_rotation = quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz)));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_relative_euler_angles_pyr(transform_t *transform, float p, float y, float r) {
  p = math_deg_to_rad(p);
  y = math_deg_to_rad(y);
  r = math_deg_to_rad(r);

  if (transform->parent) {
    quaternion_t qx = quaternion_angle_axis(p, transform->parent->local_right);
    quaternion_t qy = quaternion_angle_axis(y, transform->parent->local_up);
    quaternion_t qz = quaternion_angle_axis(r, transform->parent->local_front);

    transform->local_rotation = quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz)));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  } else {
    quaternion_t qx = quaternion_angle_axis(p, vector3_right());
    quaternion_t qy = quaternion_angle_axis(y, vector3_up());
    quaternion_t qz = quaternion_angle_axis(r, vector3_front());

    transform->local_rotation = quaternion_mul(transform->local_rotation, quaternion_mul(qy, quaternion_mul(qx, qz)));
    transform->local_rotation = quaternion_norm(transform->local_rotation);

    transform->local_right = quaternion_right(transform->local_rotation);
    transform->local_up = quaternion_up(transform->local_rotation);
    transform->local_front = quaternion_front(transform->local_rotation);

    transform->local_left = vector3_negate(transform->local_right);
    transform->local_down = vector3_negate(transform->local_up);
    transform->local_back = vector3_negate(transform->local_front);
  }

  transform_compute_world_rotation(transform);
  transform_compute_world_position(transform);
}
void transform_set_scale(transform_t *transform, vector3_t scale) {
  transform->local_scale = scale;

  transform_compute_world_scale(transform);
}
void transform_set_scale_xyz(transform_t *transform, float x, float y, float z) {
  transform->local_scale.x = x;
  transform->local_scale.y = y;
  transform->local_scale.z = z;

  transform_compute_world_scale(transform);
}
void transform_set_relative_scale(transform_t *transform, vector3_t scale) {
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
