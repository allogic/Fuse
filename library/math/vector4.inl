__forceinline math_vector4_t math_vector4_zero(void) {
  math_vector4_t v = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_one(void) {
  math_vector4_t v = {
    1.0F,
    1.0F,
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_xyzw(float x, float y, float z, float w) {
  math_vector4_t v = {
    x,
    y,
    z,
    w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_negate(math_vector4_t a) {
  math_vector4_t v = {
    -a.x,
    -a.y,
    -a.z,
    -a.w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_add(math_vector4_t a, math_vector4_t b) {
  math_vector4_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_sub(math_vector4_t a, math_vector4_t b) {
  math_vector4_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
    a.w - b.w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_mul(math_vector4_t a, math_vector4_t b) {
  math_vector4_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
    a.w * b.w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_div(math_vector4_t a, math_vector4_t b) {
  math_vector4_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
    a.w / b.w,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_add_scalar(math_vector4_t a, float b) {
  math_vector4_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_sub_scalar(math_vector4_t a, float b) {
  math_vector4_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
    a.w - b,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_mul_scalar(math_vector4_t a, float b) {
  math_vector4_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_div_scalar(math_vector4_t a, float b) {
  math_vector4_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
    a.w / b,
  };

  return v;
}
__forceinline math_vector4_t math_vector4_norm(math_vector4_t a) {
  float l = math_vector4_length(a);

  if (l > 0.0F) {
    return math_vector4_mul_scalar(a, 1.0F / l);
  } else {
    return math_vector4_zero();
  }
}
__forceinline float math_vector4_dot(math_vector4_t a, math_vector4_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float math_vector4_length(math_vector4_t a) {
  return sqrtf(math_vector4_dot(a, a));
}
__forceinline float math_vector4_length2(math_vector4_t a) {
  return math_vector4_dot(a, a);
}
__forceinline void math_vector4_print(math_vector4_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
