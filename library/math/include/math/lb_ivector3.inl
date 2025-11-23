__forceinline lb_ivector3_t lb_ivector3_zero(void) {
  lb_ivector3_t v = {
    0,
    0,
    0,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_one(void) {
  lb_ivector3_t v = {
    1,
    1,
    1,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_right(void) {
  lb_ivector3_t v = {
    1,
    0,
    0,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_up(void) {
  lb_ivector3_t v = {
    0,
    1,
    0,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_front(void) {
  lb_ivector3_t v = {
    0,
    0,
    1,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_left(void) {
  lb_ivector3_t v = {
    -1,
    0,
    0,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_down(void) {
  lb_ivector3_t v = {
    0,
    -1,
    0,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_back(void) {
  lb_ivector3_t v = {
    0,
    0,
    -1,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_xyz(int32_t x, int32_t y, int32_t z) {
  lb_ivector3_t v = {
    x,
    y,
    z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_negate(lb_ivector3_t a) {
  lb_ivector3_t v = {
    -a.x,
    -a.y,
    -a.z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_add(lb_ivector3_t a, lb_ivector3_t b) {
  lb_ivector3_t v = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_sub(lb_ivector3_t a, lb_ivector3_t b) {
  lb_ivector3_t v = {
    a.x - b.x,
    a.y - b.y,
    a.z - b.z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_mul(lb_ivector3_t a, lb_ivector3_t b) {
  lb_ivector3_t v = {
    a.x * b.x,
    a.y * b.y,
    a.z * b.z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_div(lb_ivector3_t a, lb_ivector3_t b) {
  lb_ivector3_t v = {
    a.x / b.x,
    a.y / b.y,
    a.z / b.z,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_adds(lb_ivector3_t a, int32_t b) {
  lb_ivector3_t v = {
    a.x + b,
    a.y + b,
    a.z + b,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_subs(lb_ivector3_t a, int32_t b) {
  lb_ivector3_t v = {
    a.x - b,
    a.y - b,
    a.z - b,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_muls(lb_ivector3_t a, int32_t b) {
  lb_ivector3_t v = {
    a.x * b,
    a.y * b,
    a.z * b,
  };

  return v;
}
__forceinline lb_ivector3_t lb_ivector3_divs(lb_ivector3_t a, int32_t b) {
  lb_ivector3_t v = {
    a.x / b,
    a.y / b,
    a.z / b,
  };

  return v;
}
__forceinline float lb_ivector3_dot(lb_ivector3_t a, lb_ivector3_t b) {
  return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}
__forceinline float lb_ivector3_length(lb_ivector3_t a) {
  return sqrtf(lb_ivector3_dot(a, a));
}
__forceinline float lb_ivector3_length2(lb_ivector3_t a) {
  return lb_ivector3_dot(a, a);
}
__forceinline void lb_ivector3_print(lb_ivector3_t a) {
  printf("[%d, %d, %d]\n", a.x, a.y, a.z);
}
