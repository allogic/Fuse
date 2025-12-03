__forceinline eg_vector3_t eg_vector3_zero(void) {
  eg_vector3_t v = {
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_one(void) {
  eg_vector3_t v = {
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_right(void) {
  eg_vector3_t v = {
    1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_up(void) {
  eg_vector3_t v = {
    0.0F,
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_front(void) {
  eg_vector3_t v = {
    0.0F,
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_left(void) {
  eg_vector3_t v = {
    -1.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_down(void) {
  eg_vector3_t v = {
    0.0F,
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_back(void) {
  eg_vector3_t v = {
    0.0F,
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_xyz(float x, float y, float z) {
  eg_vector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_negate(eg_vector3_t a) {
  eg_vector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_add(eg_vector3_t a, eg_vector3_t b) {
  eg_vector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_sub(eg_vector3_t a, eg_vector3_t b) {
  eg_vector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_mul(eg_vector3_t a, eg_vector3_t b) {
  eg_vector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_div(eg_vector3_t a, eg_vector3_t b) {
  eg_vector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_adds(eg_vector3_t a, float b) {
  eg_vector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_subs(eg_vector3_t a, float b) {
  eg_vector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_muls(eg_vector3_t a, float b) {
  eg_vector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_divs(eg_vector3_t a, float b) {
  eg_vector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_norm(eg_vector3_t a) {
  float l = eg_vector3_length(a);

  if (l > 0.0F) {
    return eg_vector3_muls(a, 1.0F / l);
  } else {
    return eg_vector3_zero();
  }
}
__forceinline eg_vector3_t eg_vector3_cross(eg_vector3_t a, eg_vector3_t b) {
  eg_vector3_t v = {
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x),
  };

  return v;
}
__forceinline eg_vector3_t eg_vector3_rotate(eg_vector3_t a, eg_quaternion_t b) {
  float xx = b.x * b.x;
  float yy = b.y * b.y;
  float zz = b.z * b.z;

  float xy = b.x * b.y;
  float xz = b.x * b.z;
  float yz = b.y * b.z;

  float wx = b.w * b.x;
  float wy = b.w * b.y;
  float wz = b.w * b.z;

  eg_vector3_t v = {
    (1.0F - 2.0F * (yy + zz)) * a.x + 2.0F * (xy - wz) * a.y + 2.0F * (xz + wy) * a.z,
    2.0F * (xy + wz) * a.x + (1.0F - 2.0F * (xx + zz)) * a.y + 2.0F * (yz - wx) * a.z,
    2.0F * (xz - wy) * a.x + 2.0F * (yz + wx) * a.y + (1.0F - 2.0F * (xx + yy)) * a.z,
  };

  return v;
}
__forceinline float eg_vector3_dot(eg_vector3_t a, eg_vector3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
__forceinline float eg_vector3_length(eg_vector3_t a) {
  return sqrtf(eg_vector3_dot(a, a));
}
__forceinline float eg_vector3_length2(eg_vector3_t a) {
  return eg_vector3_dot(a, a);
}
__forceinline void eg_vector3_print(eg_vector3_t a) {
  printf("[%f, %f, %f]\n", a.x, a.y, a.z);
}
