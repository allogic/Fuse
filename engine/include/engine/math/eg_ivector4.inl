__forceinline eg_ivector4_t eg_ivector4_zero(void) {
  eg_ivector4_t v = {
    0,
    0,
    0,
    0,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_one(void) {
  eg_ivector4_t v = {
    1,
    1,
    1,
    1,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w) {
  eg_ivector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_negate(eg_ivector4_t a) {
  eg_ivector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_add(eg_ivector4_t a, eg_ivector4_t b) {
  eg_ivector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_sub(eg_ivector4_t a, eg_ivector4_t b) {
  eg_ivector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_mul(eg_ivector4_t a, eg_ivector4_t b) {
  eg_ivector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_div(eg_ivector4_t a, eg_ivector4_t b) {
  eg_ivector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_adds(eg_ivector4_t a, int32_t b) {
  eg_ivector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_subs(eg_ivector4_t a, int32_t b) {
  eg_ivector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_muls(eg_ivector4_t a, int32_t b) {
  eg_ivector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline eg_ivector4_t eg_ivector4_divs(eg_ivector4_t a, int32_t b) {
  eg_ivector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline float eg_ivector4_dot(eg_ivector4_t a, eg_ivector4_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}
__forceinline float eg_ivector4_length(eg_ivector4_t a) {
  return sqrtf(eg_ivector4_dot(a, a));
}
__forceinline float eg_ivector4_length2(eg_ivector4_t a) {
  return eg_ivector4_dot(a, a);
}
__forceinline void eg_ivector4_print(eg_ivector4_t a) {
  printf("[%d, %d, %d, %d]\n", a.x, a.y, a.z, a.w);
}
