__forceinline lb_ivector4_t lb_ivector4_zero(void) {
  lb_ivector4_t v = {
    0,
    0,
    0,
    0,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_one(void) {
  lb_ivector4_t v = {
    1,
    1,
    1,
    1,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w) {
  lb_ivector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_negate(lb_ivector4_t a) {
  lb_ivector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_add(lb_ivector4_t a, lb_ivector4_t b) {
  lb_ivector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_sub(lb_ivector4_t a, lb_ivector4_t b) {
  lb_ivector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_mul(lb_ivector4_t a, lb_ivector4_t b) {
  lb_ivector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_div(lb_ivector4_t a, lb_ivector4_t b) {
  lb_ivector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_adds(lb_ivector4_t a, int32_t b) {
  lb_ivector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_subs(lb_ivector4_t a, int32_t b) {
  lb_ivector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_muls(lb_ivector4_t a, int32_t b) {
  lb_ivector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline lb_ivector4_t lb_ivector4_divs(lb_ivector4_t a, int32_t b) {
  lb_ivector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline float lb_ivector4_dot(lb_ivector4_t a, lb_ivector4_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}
__forceinline float lb_ivector4_length(lb_ivector4_t a) {
  return sqrtf(lb_ivector4_dot(a, a));
}
__forceinline float lb_ivector4_length2(lb_ivector4_t a) {
  return lb_ivector4_dot(a, a);
}
__forceinline void lb_ivector4_print(lb_ivector4_t a) {
  printf("[%d, %d, %d, %d]\n", a.x, a.y, a.z, a.w);
}
