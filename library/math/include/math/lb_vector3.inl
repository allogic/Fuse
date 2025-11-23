__forceinline lb_vector3_t lb_vector3_zero(void) {
  lb_vector3_t v = {
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_one(void) {
  lb_vector3_t v = {
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_right(void) {
  lb_vector3_t v = {
    1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_up(void) {
  lb_vector3_t v = {
    0.0F,
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_front(void) {
  lb_vector3_t v = {
    0.0F,
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_left(void) {
  lb_vector3_t v = {
    -1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_down(void) {
  lb_vector3_t v = {
    0.0F,
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_back(void) {
  lb_vector3_t v = {
    0.0F,
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_xyz(float x, float y, float z) {
  lb_vector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_negate(lb_vector3_t a) {
  lb_vector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_add(lb_vector3_t a, lb_vector3_t b) {
  lb_vector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_sub(lb_vector3_t a, lb_vector3_t b) {
  lb_vector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_mul(lb_vector3_t a, lb_vector3_t b) {
  lb_vector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_div(lb_vector3_t a, lb_vector3_t b) {
  lb_vector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_adds(lb_vector3_t a, float b) {
  lb_vector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_subs(lb_vector3_t a, float b) {
  lb_vector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_muls(lb_vector3_t a, float b) {
  lb_vector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_divs(lb_vector3_t a, float b) {
  lb_vector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_norm(lb_vector3_t a) {
  float l = lb_vector3_length(a);

  if (l > 0.0F) {
    return lb_vector3_muls(a, 1.0F / l);
  } else {
    return lb_vector3_zero();
  }
}
__forceinline lb_vector3_t lb_vector3_cross(lb_vector3_t a, lb_vector3_t b) {
  lb_vector3_t v = {
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x),
  };

  return v;
}
__forceinline lb_vector3_t lb_vector3_rotate(lb_vector3_t a, lb_quaternion_t b) {
  float xx = b.x * b.x;
  float yy = b.y * b.y;
  float zz = b.z * b.z;

  float xy = b.x * b.y;
  float xz = b.x * b.z;
  float yz = b.y * b.z;

  float wx = b.w * b.x;
  float wy = b.w * b.y;
  float wz = b.w * b.z;

  lb_vector3_t v = {
    (1.0F - 2.0F * (yy + zz)) * a.x + 2.0F * (xy - wz) * a.y + 2.0F * (xz + wy) * a.z,
    2.0F * (xy + wz) * a.x + (1.0F - 2.0F * (xx + zz)) * a.y + 2.0F * (yz - wx) * a.z,
    2.0F * (xz - wy) * a.x + 2.0F * (yz + wx) * a.y + (1.0F - 2.0F * (xx + yy)) * a.z,
  };

  return v;
}
__forceinline float lb_vector3_dot(lb_vector3_t a, lb_vector3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
__forceinline float lb_vector3_length(lb_vector3_t a) {
  return sqrtf(lb_vector3_dot(a, a));
}
__forceinline float lb_vector3_length2(lb_vector3_t a) {
  return lb_vector3_dot(a, a);
}
__forceinline void lb_vector3_print(lb_vector3_t a) {
  printf("[%f, %f, %f]\n", a.x, a.y, a.z);
}
