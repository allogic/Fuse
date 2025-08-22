__forceinline math_ivector4_t math_ivector4_zero(void) {
  math_ivector4_t v = {
    0,
    0,
    0,
    0,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_one(void) {
  math_ivector4_t v = {
    1,
    1,
    1,
    1,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w) {
  math_ivector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_negate(math_ivector4_t a) {
  math_ivector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_add(math_ivector4_t a, math_ivector4_t b) {
  math_ivector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_sub(math_ivector4_t a, math_ivector4_t b) {
  math_ivector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_mul(math_ivector4_t a, math_ivector4_t b) {
  math_ivector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_div(math_ivector4_t a, math_ivector4_t b) {
  math_ivector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_add_scalar(math_ivector4_t a, int32_t b) {
  math_ivector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_sub_scalar(math_ivector4_t a, int32_t b) {
  math_ivector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_mul_scalar(math_ivector4_t a, int32_t b) {
  math_ivector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline math_ivector4_t math_ivector4_div_scalar(math_ivector4_t a, int32_t b) {
  math_ivector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline float math_ivector4_dot(math_ivector4_t a, math_ivector4_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}
__forceinline float math_ivector4_length(math_ivector4_t a) {
  return sqrtf(math_ivector4_dot(a, a));
}
__forceinline float math_ivector4_length2(math_ivector4_t a) {
  return math_ivector4_dot(a, a);
}
__forceinline void math_ivector4_print(math_ivector4_t a) {
  printf("[%d, %d, %d, %d]\n", a.x, a.y, a.z, a.w);
}
