__forceinline lb_vector2_t lb_vector2_zero(void) {
  lb_vector2_t v = {
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_one(void) {
  lb_vector2_t v = {
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_right(void) {
  lb_vector2_t v = {
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_up(void) {
  lb_vector2_t v = {
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_left(void) {
  lb_vector2_t v = {
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_down(void) {
  lb_vector2_t v = {
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_xy(float x, float y) {
  lb_vector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_negate(lb_vector2_t a) {
  lb_vector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_add(lb_vector2_t a, lb_vector2_t b) {
  lb_vector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_sub(lb_vector2_t a, lb_vector2_t b) {
  lb_vector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_mul(lb_vector2_t a, lb_vector2_t b) {
  lb_vector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_div(lb_vector2_t a, lb_vector2_t b) {
  lb_vector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_adds(lb_vector2_t a, float b) {
  lb_vector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_subs(lb_vector2_t a, float b) {
  lb_vector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_muls(lb_vector2_t a, float b) {
  lb_vector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_divs(lb_vector2_t a, float b) {
  lb_vector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline lb_vector2_t lb_vector2_norm(lb_vector2_t a) {
  float l = lb_vector2_length(a);

  if (l > 0.0F) {
    return lb_vector2_muls(a, 1.0F / l);
  } else {
    return lb_vector2_zero();
  }
}
__forceinline float lb_vector2_dot(lb_vector2_t a, lb_vector2_t b) {
  return (a.x * b.x) + (a.y * b.y);
}
__forceinline float lb_vector2_length(lb_vector2_t a) {
  return sqrtf(lb_vector2_dot(a, a));
}
__forceinline float lb_vector2_length2(lb_vector2_t a) {
  return lb_vector2_dot(a, a);
}
__forceinline void lb_vector2_print(lb_vector2_t a) {
  printf("[%f, %f]\n", a.x, a.y);
}
