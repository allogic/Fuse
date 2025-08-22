__forceinline math_ivector3_t math_ivector3_zero(void) {
  math_ivector3_t v = {
    0,
    0,
    0,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_one(void) {
  math_ivector3_t v = {
    1,
    1,
    1,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_right(void) {
  math_ivector3_t v = {
    1,
    0,
    0,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_up(void) {
  math_ivector3_t v = {
    0,
    1,
    0,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_front(void) {
  math_ivector3_t v = {
    0,
    0,
    1,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_left(void) {
  math_ivector3_t v = {
    -1,
    0,
    0,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_down(void) {
  math_ivector3_t v = {
    0,
    -1,
    0,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_back(void) {
  math_ivector3_t v = {
    0,
    0,
    -1,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_xyz(int32_t x, int32_t y, int32_t z) {
  math_ivector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_negate(math_ivector3_t a) {
  math_ivector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_add(math_ivector3_t a, math_ivector3_t b) {
  math_ivector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_sub(math_ivector3_t a, math_ivector3_t b) {
  math_ivector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_mul(math_ivector3_t a, math_ivector3_t b) {
  math_ivector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_div(math_ivector3_t a, math_ivector3_t b) {
  math_ivector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_add_scalar(math_ivector3_t a, int32_t b) {
  math_ivector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_sub_scalar(math_ivector3_t a, int32_t b) {
  math_ivector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_mul_scalar(math_ivector3_t a, int32_t b) {
  math_ivector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline math_ivector3_t math_ivector3_div_scalar(math_ivector3_t a, int32_t b) {
  math_ivector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline float math_ivector3_dot(math_ivector3_t a, math_ivector3_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}
__forceinline float math_ivector3_length(math_ivector3_t a) {
  return sqrtf(math_ivector3_dot(a, a));
}
__forceinline float math_ivector3_length2(math_ivector3_t a) {
  return math_ivector3_dot(a, a);
}
__forceinline void math_ivector3_print(math_ivector3_t a) {
  printf("[%d, %d, %d]\n", a.x, a.y, a.z);
}
