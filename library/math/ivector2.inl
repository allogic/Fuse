__forceinline math_ivector2_t math_ivector2_zero(void) {
  math_ivector2_t v = {
    0,
    0,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_one(void) {
  math_ivector2_t v = {
    1,
    1,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_right(void) {
  math_ivector2_t v = {
    1,
    0,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_up(void) {
  math_ivector2_t v = {
    0,
    1,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_left(void) {
  math_ivector2_t v = {
    -1,
    0,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_down(void) {
  math_ivector2_t v = {
    0,
    -1,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_xy(int32_t x, int32_t y) {
  math_ivector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_negate(math_ivector2_t a) {
  math_ivector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_add(math_ivector2_t a, math_ivector2_t b) {
  math_ivector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_sub(math_ivector2_t a, math_ivector2_t b) {
  math_ivector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_mul(math_ivector2_t a, math_ivector2_t b) {
  math_ivector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_div(math_ivector2_t a, math_ivector2_t b) {
  math_ivector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_add_scalar(math_ivector2_t a, int32_t b) {
  math_ivector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_sub_scalar(math_ivector2_t a, int32_t b) {
  math_ivector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_mul_scalar(math_ivector2_t a, int32_t b) {
  math_ivector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline math_ivector2_t math_ivector2_div_scalar(math_ivector2_t a, int32_t b) {
  math_ivector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline float math_ivector2_dot(math_ivector2_t a, math_ivector2_t b) {
  return (float)((a.x * b.x) + (a.y * b.y));
}
__forceinline float math_ivector2_length(math_ivector2_t a) {
  return sqrtf(math_ivector2_dot(a, a));
}
__forceinline float math_ivector2_length2(math_ivector2_t a) {
  return math_ivector2_dot(a, a);
}
__forceinline void math_ivector2_print(math_ivector2_t a) {
  printf("[%d, %d]\n", a.x, a.y);
}
