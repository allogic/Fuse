__forceinline lb_quaternion_t lb_quaternion_zero(void) {
  lb_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_identity(void) {
  lb_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_xyzw(float x, float y, float z, float w) {
  lb_quaternion_t q = {
    x,
    y,
    z,
    w,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_add(lb_quaternion_t a, lb_quaternion_t b) {
  lb_quaternion_t q = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_adds(lb_quaternion_t a, float b) {
  lb_quaternion_t q = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_mul(lb_quaternion_t a, lb_quaternion_t b) {
  lb_quaternion_t q = {
    (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
    (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
    (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
    (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_muls(lb_quaternion_t a, float b) {
  lb_quaternion_t q = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_conjugate(lb_quaternion_t a) {
  lb_quaternion_t q = {
    -a.x,
    -a.y,
    -a.z,
    a.w,
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_right(lb_quaternion_t a) {
  lb_vector3_t q = {
    1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)),
    2.0F * ((a.x * a.y) + (a.w * a.z)),
    2.0F * ((a.x * a.z) - (a.w * a.y)),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_up(lb_quaternion_t a) {
  lb_vector3_t q = {
    2.0F * ((a.x * a.y) - (a.w * a.z)),
    1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z)),
    2.0F * ((a.y * a.z) + (a.w * a.x)),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_front(lb_quaternion_t a) {
  lb_vector3_t q = {
    2.0F * ((a.x * a.z) + (a.w * a.y)),
    2.0F * ((a.y * a.z) - (a.w * a.x)),
    1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_left(lb_quaternion_t a) {
  lb_vector3_t q = {
    -(1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z))),
    -(2.0F * ((a.x * a.y) + (a.w * a.z))),
    -(2.0F * ((a.x * a.z) - (a.w * a.y))),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_down(lb_quaternion_t a) {
  lb_vector3_t q = {
    -(2.0F * ((a.x * a.y) - (a.w * a.z))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z))),
    -(2.0F * ((a.y * a.z) + (a.w * a.x))),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_back(lb_quaternion_t a) {
  lb_vector3_t q = {
    -(2.0F * ((a.x * a.z) + (a.w * a.y))),
    -(2.0F * ((a.y * a.z) - (a.w * a.x))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y))),
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_to_euler_angles(lb_quaternion_t a) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (a.w * a.x) - (a.y * a.z);

  if (test > (0.5F - LB_EPSILON_6)) {
    pitch = LB_PI_HALF;
    yaw = 2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else if (test < -(0.5F - LB_EPSILON_6)) {
    pitch = -LB_PI_HALF;
    yaw = -2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((a.w * a.y) - (a.x * a.z)), 1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)));
    roll = atan2f(2.0F * ((a.w * a.z) - (a.x * a.y)), 1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)));
  }

  lb_vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline lb_vector3_t lb_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (w * x) - (y * z);

  if (test > (0.5F - LB_EPSILON_6)) {
    pitch = LB_PI_HALF;
    yaw = 2.0F * atan2f(z, w);
    roll = 0.0F;
  } else if (test < -(0.5F - LB_EPSILON_6)) {
    pitch = -LB_PI_HALF;
    yaw = -2.0F * atan2f(z, w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((w * y) - (x * z)), 1.0F - 2.0F * ((x * x) + (y * y)));
    roll = atan2f(2.0F * ((w * z) - (x * y)), 1.0F - 2.0F * ((y * y) + (z * z)));
  }

  lb_vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_from_euler_angles(lb_vector3_t a) {
  float pitch = lb_deg_to_rad(a.x);
  float yaw = lb_deg_to_rad(a.y);
  float roll = lb_deg_to_rad(a.z);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  lb_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_from_euler_angles_pyr(float p, float y, float r) {
  float pitch = lb_deg_to_rad(p);
  float yaw = lb_deg_to_rad(y);
  float roll = lb_deg_to_rad(r);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  lb_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_angle_axis(float a, lb_vector3_t b) {
  lb_vector3_t n = lb_vector3_norm(b);

  float a_half = a * 0.5F;
  float s = sinf(a_half);

  lb_quaternion_t q = {
    n.x * s,
    n.y * s,
    n.z * s,
    cosf(a_half),
  };

  return q;
}
__forceinline lb_quaternion_t lb_quaternion_norm(lb_quaternion_t a) {
  float l = lb_quaternion_length(a);

  if (l > 0.0F) {
    return lb_quaternion_muls(a, 1.0F / l);
  } else {
    return lb_quaternion_identity();
  }
}
__forceinline float lb_quaternion_dot(lb_quaternion_t a, lb_quaternion_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float lb_quaternion_length(lb_quaternion_t a) {
  return sqrtf(lb_quaternion_dot(a, a));
}
__forceinline float lb_quaternion_length2(lb_quaternion_t a) {
  return lb_quaternion_dot(a, a);
}
__forceinline void lb_quaternion_print(lb_quaternion_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
