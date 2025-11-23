__forceinline lb_ivector2_t lb_ivector2_zero(void) {
  lb_ivector2_t v = {
    0,
    0,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_one(void) {
  lb_ivector2_t v = {
    1,
    1,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_right(void) {
  lb_ivector2_t v = {
    1,
    0,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_up(void) {
  lb_ivector2_t v = {
    0,
    1,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_left(void) {
  lb_ivector2_t v = {
    -1,
    0,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_down(void) {
  lb_ivector2_t v = {
    0,
    -1,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_xy(int32_t x, int32_t y) {
  lb_ivector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_negate(lb_ivector2_t a) {
  lb_ivector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_add(lb_ivector2_t a, lb_ivector2_t b) {
  lb_ivector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_sub(lb_ivector2_t a, lb_ivector2_t b) {
  lb_ivector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_mul(lb_ivector2_t a, lb_ivector2_t b) {
  lb_ivector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_div(lb_ivector2_t a, lb_ivector2_t b) {
  lb_ivector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_adds(lb_ivector2_t a, int32_t b) {
  lb_ivector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_subs(lb_ivector2_t a, int32_t b) {
  lb_ivector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_muls(lb_ivector2_t a, int32_t b) {
  lb_ivector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline lb_ivector2_t lb_ivector2_divs(lb_ivector2_t a, int32_t b) {
  lb_ivector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline float lb_ivector2_dot(lb_ivector2_t a, lb_ivector2_t b) {
  return (float)((a.x * b.x) + (a.y * b.y));
}
__forceinline float lb_ivector2_length(lb_ivector2_t a) {
  return sqrtf(lb_ivector2_dot(a, a));
}
__forceinline float lb_ivector2_length2(lb_ivector2_t a) {
  return lb_ivector2_dot(a, a);
}
__forceinline void lb_ivector2_print(lb_ivector2_t a) {
  printf("[%d, %d]\n", a.x, a.y);
}
