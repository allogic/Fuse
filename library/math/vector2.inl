__forceinline math_vector2_t math_vector2_zero(void) {
  math_vector2_t v = {
    0.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_one(void) {
  math_vector2_t v = {
    1.0F,
    1.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_right(void) {
  math_vector2_t v = {
    1.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_up(void) {
  math_vector2_t v = {
    0.0F,
    1.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_left(void) {
  math_vector2_t v = {
    -1.0F,
    0.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_down(void) {
  math_vector2_t v = {
    0.0F,
    -1.0F,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_xy(float x, float y) {
  math_vector2_t v = {
    x,
    y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_negate(math_vector2_t a) {
  math_vector2_t v = {
    -a.x,
    -a.y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_add(math_vector2_t a, math_vector2_t b) {
  math_vector2_t v = {
    a.x + b.x,
    a.y + b.y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_sub(math_vector2_t a, math_vector2_t b) {
  math_vector2_t v = {
    a.x - b.x,
    a.y - b.y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_mul(math_vector2_t a, math_vector2_t b) {
  math_vector2_t v = {
    a.x * b.x,
    a.y * b.y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_div(math_vector2_t a, math_vector2_t b) {
  math_vector2_t v = {
    a.x / b.x,
    a.y / b.y,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_add_scalar(math_vector2_t a, float b) {
  math_vector2_t v = {
    a.x + b,
    a.y + b,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_sub_scalar(math_vector2_t a, float b) {
  math_vector2_t v = {
    a.x - b,
    a.y - b,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_mul_scalar(math_vector2_t a, float b) {
  math_vector2_t v = {
    a.x * b,
    a.y * b,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_div_scalar(math_vector2_t a, float b) {
  math_vector2_t v = {
    a.x / b,
    a.y / b,
  };

  return v;
}
__forceinline math_vector2_t math_vector2_norm(math_vector2_t a) {
  float l = math_vector2_length(a);

  if (l > 0.0F) {
    return math_vector2_mul_scalar(a, 1.0F / l);
  } else {
    return math_vector2_zero();
  }
}
__forceinline float math_vector2_dot(math_vector2_t a, math_vector2_t b) {
  return (a.x * b.x) + (a.y * b.y);
}
__forceinline float math_vector2_length(math_vector2_t a) {
  return sqrtf(math_vector2_dot(a, a));
}
__forceinline float math_vector2_length2(math_vector2_t a) {
  return math_vector2_dot(a, a);
}
__forceinline void math_vector2_print(math_vector2_t a) {
  printf("[%f, %f]\n", a.x, a.y);
}
