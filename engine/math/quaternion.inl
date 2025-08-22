__forceinline math_quaternion_t math_quaternion_zero(void) {
  math_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_identity(void) {
  math_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_xyzw(float x, float y, float z, float w) {
  math_quaternion_t q = {
    x,
    y,
    z,
    w,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_mul(math_quaternion_t a, math_quaternion_t b) {
  math_quaternion_t q = {
    (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
    (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
    (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
    (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_mul_scalar(math_quaternion_t a, float b) {
  math_quaternion_t q = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_conjugate(math_quaternion_t a) {
  math_quaternion_t q = {
    -a.x,
    -a.y,
    -a.z,
    a.w,
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_right(math_quaternion_t a) {
  math_vector3_t q = {
    1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)),
    2.0F * ((a.x * a.y) + (a.w * a.z)),
    2.0F * ((a.x * a.z) - (a.w * a.y)),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_up(math_quaternion_t a) {
  math_vector3_t q = {
    2.0F * ((a.x * a.y) - (a.w * a.z)),
    1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z)),
    2.0F * ((a.y * a.z) + (a.w * a.x)),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_front(math_quaternion_t a) {
  math_vector3_t q = {
    2.0F * ((a.x * a.z) + (a.w * a.y)),
    2.0F * ((a.y * a.z) - (a.w * a.x)),
    1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_left(math_quaternion_t a) {
  math_vector3_t q = {
    -(1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z))),
    -(2.0F * ((a.x * a.y) + (a.w * a.z))),
    -(2.0F * ((a.x * a.z) - (a.w * a.y))),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_down(math_quaternion_t a) {
  math_vector3_t q = {
    -(2.0F * ((a.x * a.y) - (a.w * a.z))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z))),
    -(2.0F * ((a.y * a.z) + (a.w * a.x))),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_back(math_quaternion_t a) {
  math_vector3_t q = {
    -(2.0F * ((a.x * a.z) + (a.w * a.y))),
    -(2.0F * ((a.y * a.z) - (a.w * a.x))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y))),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_to_euler_angles(math_quaternion_t a) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (a.w * a.x) - (a.y * a.z);

  if (test > (0.5F - MATH_EPSILON_6)) {
    pitch = MATH_PI_HALF;
    yaw = 2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else if (test < -(0.5F - MATH_EPSILON_6)) {
    pitch = -MATH_PI_HALF;
    yaw = -2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((a.w * a.y) - (a.x * a.z)), 1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)));
    roll = atan2f(2.0F * ((a.w * a.z) - (a.x * a.y)), 1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)));
  }

  math_vector3_t q = {
    math_utility_rad_to_deg(pitch),
    math_utility_rad_to_deg(yaw),
    math_utility_rad_to_deg(roll),
  };

  return q;
}
__forceinline math_vector3_t math_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (w * x) - (y * z);

  if (test > (0.5F - MATH_EPSILON_6)) {
    pitch = MATH_PI_HALF;
    yaw = 2.0F * atan2f(z, w);
    roll = 0.0F;
  } else if (test < -(0.5F - MATH_EPSILON_6)) {
    pitch = -MATH_PI_HALF;
    yaw = -2.0F * atan2f(z, w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((w * y) - (x * z)), 1.0F - 2.0F * ((x * x) + (y * y)));
    roll = atan2f(2.0F * ((w * z) - (x * y)), 1.0F - 2.0F * ((y * y) + (z * z)));
  }

  math_vector3_t q = {
    math_utility_rad_to_deg(pitch),
    math_utility_rad_to_deg(yaw),
    math_utility_rad_to_deg(roll),
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_from_euler_angles(math_vector3_t a) {
  float pitch = math_utility_deg_to_rad(a.x);
  float yaw = math_utility_deg_to_rad(a.y);
  float roll = math_utility_deg_to_rad(a.z);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  math_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_from_euler_angles_pyr(float p, float y, float r) {
  float pitch = math_utility_deg_to_rad(p);
  float yaw = math_utility_deg_to_rad(y);
  float roll = math_utility_deg_to_rad(r);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  math_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_angle_axis(float a, math_vector3_t b) {
  math_vector3_t n = math_vector3_norm(b);

  float a_half = a * 0.5F;
  float s = sinf(a_half);

  math_quaternion_t q = {
    n.x * s,
    n.y * s,
    n.z * s,
    cosf(a_half),
  };

  return q;
}
__forceinline math_quaternion_t math_quaternion_norm(math_quaternion_t a) {
  float l = math_quaternion_length(a);

  if (l > 0.0F) {
    return math_quaternion_mul_scalar(a, 1.0F / l);
  } else {
    return math_quaternion_identity();
  }
}
__forceinline float math_quaternion_dot(math_quaternion_t a, math_quaternion_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float math_quaternion_length(math_quaternion_t a) {
  return sqrtf(math_quaternion_dot(a, a));
}
__forceinline float math_quaternion_length2(math_quaternion_t a) {
  return math_quaternion_dot(a, a);
}
__forceinline void math_quaternion_print(math_quaternion_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
