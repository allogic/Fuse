__forceinline lb_vector4_t lb_vector4_zero(void) {
  lb_vector4_t v = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_one(void) {
  lb_vector4_t v = {
    1.0F,
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_xyzw(float x, float y, float z, float w) {
  lb_vector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_negate(lb_vector4_t a) {
  lb_vector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_add(lb_vector4_t a, lb_vector4_t b) {
  lb_vector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_sub(lb_vector4_t a, lb_vector4_t b) {
  lb_vector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_mul(lb_vector4_t a, lb_vector4_t b) {
  lb_vector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_div(lb_vector4_t a, lb_vector4_t b) {
  lb_vector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_adds(lb_vector4_t a, float b) {
  lb_vector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_subs(lb_vector4_t a, float b) {
  lb_vector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_muls(lb_vector4_t a, float b) {
  lb_vector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_divs(lb_vector4_t a, float b) {
  lb_vector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline lb_vector4_t lb_vector4_norm(lb_vector4_t a) {
  float l = lb_vector4_length(a);

  if (l > 0.0F) {
    return lb_vector4_muls(a, 1.0F / l);
  } else {
    return lb_vector4_zero();
  }
}
__forceinline float lb_vector4_dot(lb_vector4_t a, lb_vector4_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float lb_vector4_length(lb_vector4_t a) {
  return sqrtf(lb_vector4_dot(a, a));
}
__forceinline float lb_vector4_length2(lb_vector4_t a) {
  return lb_vector4_dot(a, a);
}
__forceinline void lb_vector4_print(lb_vector4_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
