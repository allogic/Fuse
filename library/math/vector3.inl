__forceinline math_vector3_t math_vector3_zero(void) {
  math_vector3_t v = {
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_one(void) {
  math_vector3_t v = {
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_right(void) {
  math_vector3_t v = {
    1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_up(void) {
  math_vector3_t v = {
    0.0F,
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_front(void) {
  math_vector3_t v = {
    0.0F,
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_left(void) {
  math_vector3_t v = {
    -1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_down(void) {
  math_vector3_t v = {
    0.0F,
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_back(void) {
  math_vector3_t v = {
    0.0F,
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_xyz(float x, float y, float z) {
  math_vector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_negate(math_vector3_t a) {
  math_vector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_add(math_vector3_t a, math_vector3_t b) {
  math_vector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_sub(math_vector3_t a, math_vector3_t b) {
  math_vector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_mul(math_vector3_t a, math_vector3_t b) {
  math_vector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_div(math_vector3_t a, math_vector3_t b) {
  math_vector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_add_scalar(math_vector3_t a, float b) {
  math_vector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_sub_scalar(math_vector3_t a, float b) {
  math_vector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_mul_scalar(math_vector3_t a, float b) {
  math_vector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_div_scalar(math_vector3_t a, float b) {
  math_vector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline math_vector3_t math_vector3_norm(math_vector3_t a) {
  float l = math_vector3_length(a);

  if (l > 0.0F) {
    return math_vector3_mul_scalar(a, 1.0F / l);
  } else {
    return math_vector3_zero();
  }
}
__forceinline math_vector3_t math_vector3_cross(math_vector3_t a, math_vector3_t b) {
  math_vector3_t v = {
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x),
  };

  return v;
}
__forceinline math_vector3_t math_vector3_rotate(math_vector3_t a, math_quaternion_t b) {
  float xx = b.x * b.x;
  float yy = b.y * b.y;
  float zz = b.z * b.z;

  float xy = b.x * b.y;
  float xz = b.x * b.z;
  float yz = b.y * b.z;

  float wx = b.w * b.x;
  float wy = b.w * b.y;
  float wz = b.w * b.z;

  math_vector3_t v = {
    (1.0F - 2.0F * (yy + zz)) * a.x + 2.0F * (xy - wz) * a.y + 2.0F * (xz + wy) * a.z,
    2.0F * (xy + wz) * a.x + (1.0F - 2.0F * (xx + zz)) * a.y + 2.0F * (yz - wx) * a.z,
    2.0F * (xz - wy) * a.x + 2.0F * (yz + wx) * a.y + (1.0F - 2.0F * (xx + yy)) * a.z,
  };

  return v;
}
__forceinline float math_vector3_dot(math_vector3_t a, math_vector3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
__forceinline float math_vector3_length(math_vector3_t a) {
  return sqrtf(math_vector3_dot(a, a));
}
__forceinline float math_vector3_length2(math_vector3_t a) {
  return math_vector3_dot(a, a);
}
__forceinline void math_vector3_print(math_vector3_t a) {
  printf("[%f, %f, %f]\n", a.x, a.y, a.z);
}
