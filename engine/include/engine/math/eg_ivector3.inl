__forceinline eg_ivector3_t eg_ivector3_zero(void) {
  eg_ivector3_t v = {
    0,
    0,
    0,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_one(void) {
  eg_ivector3_t v = {
    1,
    1,
    1,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_right(void) {
  eg_ivector3_t v = {
    1,
    0,
    0,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_up(void) {
  eg_ivector3_t v = {
    0,
    1,
    0,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_front(void) {
  eg_ivector3_t v = {
    0,
    0,
    1,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_left(void) {
  eg_ivector3_t v = {
    -1,
    0,
    0,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_down(void) {
  eg_ivector3_t v = {
    0,
    -1,
    0,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_back(void) {
  eg_ivector3_t v = {
    0,
    0,
    -1,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_xyz(int32_t x, int32_t y, int32_t z) {
  eg_ivector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_negate(eg_ivector3_t a) {
  eg_ivector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_add(eg_ivector3_t a, eg_ivector3_t b) {
  eg_ivector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_sub(eg_ivector3_t a, eg_ivector3_t b) {
  eg_ivector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_mul(eg_ivector3_t a, eg_ivector3_t b) {
  eg_ivector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_div(eg_ivector3_t a, eg_ivector3_t b) {
  eg_ivector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_adds(eg_ivector3_t a, int32_t b) {
  eg_ivector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_subs(eg_ivector3_t a, int32_t b) {
  eg_ivector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_muls(eg_ivector3_t a, int32_t b) {
  eg_ivector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline eg_ivector3_t eg_ivector3_divs(eg_ivector3_t a, int32_t b) {
  eg_ivector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline float eg_ivector3_dot(eg_ivector3_t a, eg_ivector3_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}
__forceinline float eg_ivector3_length(eg_ivector3_t a) {
  return sqrtf(eg_ivector3_dot(a, a));
}
__forceinline float eg_ivector3_length2(eg_ivector3_t a) {
  return eg_ivector3_dot(a, a);
}
__forceinline void eg_ivector3_print(eg_ivector3_t a) {
  printf("[%d, %d, %d]\n", a.x, a.y, a.z);
}
