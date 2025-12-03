__forceinline eg_vector4_t eg_vector4_zero(void) {
  eg_vector4_t v = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_one(void) {
  eg_vector4_t v = {
    1.0F,
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_xyzw(float x, float y, float z, float w) {
  eg_vector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_negate(eg_vector4_t a) {
  eg_vector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_add(eg_vector4_t a, eg_vector4_t b) {
  eg_vector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_sub(eg_vector4_t a, eg_vector4_t b) {
  eg_vector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_mul(eg_vector4_t a, eg_vector4_t b) {
  eg_vector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_div(eg_vector4_t a, eg_vector4_t b) {
  eg_vector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_adds(eg_vector4_t a, float b) {
  eg_vector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_subs(eg_vector4_t a, float b) {
  eg_vector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_muls(eg_vector4_t a, float b) {
  eg_vector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_divs(eg_vector4_t a, float b) {
  eg_vector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline eg_vector4_t eg_vector4_norm(eg_vector4_t a) {
  float l = eg_vector4_length(a);

  if (l > 0.0F) {
    return eg_vector4_muls(a, 1.0F / l);
  } else {
    return eg_vector4_zero();
  }
}
__forceinline float eg_vector4_dot(eg_vector4_t a, eg_vector4_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float eg_vector4_length(eg_vector4_t a) {
  return sqrtf(eg_vector4_dot(a, a));
}
__forceinline float eg_vector4_length2(eg_vector4_t a) {
  return eg_vector4_dot(a, a);
}
__forceinline void eg_vector4_print(eg_vector4_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
