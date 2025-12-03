__forceinline eg_ivector2_t eg_ivector2_zero(void) {
  eg_ivector2_t v = {
    0,
    0,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_one(void) {
  eg_ivector2_t v = {
    1,
    1,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_right(void) {
  eg_ivector2_t v = {
    1,
    0,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_up(void) {
  eg_ivector2_t v = {
    0,
    1,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_left(void) {
  eg_ivector2_t v = {
    -1,
    0,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_down(void) {
  eg_ivector2_t v = {
    0,
    -1,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_xy(int32_t x, int32_t y) {
  eg_ivector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_negate(eg_ivector2_t a) {
  eg_ivector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_add(eg_ivector2_t a, eg_ivector2_t b) {
  eg_ivector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_sub(eg_ivector2_t a, eg_ivector2_t b) {
  eg_ivector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_mul(eg_ivector2_t a, eg_ivector2_t b) {
  eg_ivector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_div(eg_ivector2_t a, eg_ivector2_t b) {
  eg_ivector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_adds(eg_ivector2_t a, int32_t b) {
  eg_ivector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_subs(eg_ivector2_t a, int32_t b) {
  eg_ivector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_muls(eg_ivector2_t a, int32_t b) {
  eg_ivector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline eg_ivector2_t eg_ivector2_divs(eg_ivector2_t a, int32_t b) {
  eg_ivector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline float eg_ivector2_dot(eg_ivector2_t a, eg_ivector2_t b) {
  return (float)((a.x * b.x) + (a.y * b.y));
}
__forceinline float eg_ivector2_length(eg_ivector2_t a) {
  return sqrtf(eg_ivector2_dot(a, a));
}
__forceinline float eg_ivector2_length2(eg_ivector2_t a) {
  return eg_ivector2_dot(a, a);
}
__forceinline void eg_ivector2_print(eg_ivector2_t a) {
  printf("[%d, %d]\n", a.x, a.y);
}
