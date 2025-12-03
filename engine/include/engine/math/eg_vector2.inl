__forceinline eg_vector2_t eg_vector2_zero(void) {
  eg_vector2_t v = {
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_one(void) {
  eg_vector2_t v = {
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_right(void) {
  eg_vector2_t v = {
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_up(void) {
  eg_vector2_t v = {
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_left(void) {
  eg_vector2_t v = {
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_down(void) {
  eg_vector2_t v = {
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_xy(float x, float y) {
  eg_vector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_negate(eg_vector2_t a) {
  eg_vector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_add(eg_vector2_t a, eg_vector2_t b) {
  eg_vector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_sub(eg_vector2_t a, eg_vector2_t b) {
  eg_vector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_mul(eg_vector2_t a, eg_vector2_t b) {
  eg_vector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_div(eg_vector2_t a, eg_vector2_t b) {
  eg_vector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_adds(eg_vector2_t a, float b) {
  eg_vector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_subs(eg_vector2_t a, float b) {
  eg_vector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_muls(eg_vector2_t a, float b) {
  eg_vector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_divs(eg_vector2_t a, float b) {
  eg_vector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline eg_vector2_t eg_vector2_norm(eg_vector2_t a) {
  float l = eg_vector2_length(a);

  if (l > 0.0F) {
    return eg_vector2_muls(a, 1.0F / l);
  } else {
    return eg_vector2_zero();
  }
}
__forceinline float eg_vector2_dot(eg_vector2_t a, eg_vector2_t b) {
  return (a.x * b.x) + (a.y * b.y);
}
__forceinline float eg_vector2_length(eg_vector2_t a) {
  return sqrtf(eg_vector2_dot(a, a));
}
__forceinline float eg_vector2_length2(eg_vector2_t a) {
  return eg_vector2_dot(a, a);
}
__forceinline void eg_vector2_print(eg_vector2_t a) {
  printf("[%f, %f]\n", a.x, a.y);
}
