__forceinline eg_quaternion_t eg_quaternion_zero(void) {
  eg_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    0.0F,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_identity(void) {
  eg_quaternion_t q = {
    0.0F,
    0.0F,
    0.0F,
    1.0F,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_xyzw(float x, float y, float z, float w) {
  eg_quaternion_t q = {
    x,
    y,
    z,
    w,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_add(eg_quaternion_t a, eg_quaternion_t b) {
  eg_quaternion_t q = {
    a.x + b.x,
    a.y + b.y,
    a.z + b.z,
    a.w + b.w,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_adds(eg_quaternion_t a, float b) {
  eg_quaternion_t q = {
    a.x + b,
    a.y + b,
    a.z + b,
    a.w + b,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_mul(eg_quaternion_t a, eg_quaternion_t b) {
  eg_quaternion_t q = {
    (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
    (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
    (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
    (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_muls(eg_quaternion_t a, float b) {
  eg_quaternion_t q = {
    a.x * b,
    a.y * b,
    a.z * b,
    a.w * b,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_conjugate(eg_quaternion_t a) {
  eg_quaternion_t q = {
    -a.x,
    -a.y,
    -a.z,
    a.w,
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_right(eg_quaternion_t a) {
  eg_vector3_t q = {
    1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)),
    2.0F * ((a.x * a.y) + (a.w * a.z)),
    2.0F * ((a.x * a.z) - (a.w * a.y)),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_up(eg_quaternion_t a) {
  eg_vector3_t q = {
    2.0F * ((a.x * a.y) - (a.w * a.z)),
    1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z)),
    2.0F * ((a.y * a.z) + (a.w * a.x)),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_front(eg_quaternion_t a) {
  eg_vector3_t q = {
    2.0F * ((a.x * a.z) + (a.w * a.y)),
    2.0F * ((a.y * a.z) - (a.w * a.x)),
    1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_left(eg_quaternion_t a) {
  eg_vector3_t q = {
    -(1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z))),
    -(2.0F * ((a.x * a.y) + (a.w * a.z))),
    -(2.0F * ((a.x * a.z) - (a.w * a.y))),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_down(eg_quaternion_t a) {
  eg_vector3_t q = {
    -(2.0F * ((a.x * a.y) - (a.w * a.z))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.z * a.z))),
    -(2.0F * ((a.y * a.z) + (a.w * a.x))),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_back(eg_quaternion_t a) {
  eg_vector3_t q = {
    -(2.0F * ((a.x * a.z) + (a.w * a.y))),
    -(2.0F * ((a.y * a.z) - (a.w * a.x))),
    -(1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y))),
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_to_euler_angles(eg_quaternion_t a) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (a.w * a.x) - (a.y * a.z);

  if (test > (0.5F - EG_EPSILON_6)) {
    pitch = EG_PI_HALF;
    yaw = 2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else if (test < -(0.5F - EG_EPSILON_6)) {
    pitch = -EG_PI_HALF;
    yaw = -2.0F * atan2f(a.z, a.w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((a.w * a.y) - (a.x * a.z)), 1.0F - 2.0F * ((a.x * a.x) + (a.y * a.y)));
    roll = atan2f(2.0F * ((a.w * a.z) - (a.x * a.y)), 1.0F - 2.0F * ((a.y * a.y) + (a.z * a.z)));
  }

  eg_vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline eg_vector3_t eg_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w) {
  float pitch = 0.0F;
  float yaw = 0.0F;
  float roll = 0.0F;

  float test = (w * x) - (y * z);

  if (test > (0.5F - EG_EPSILON_6)) {
    pitch = EG_PI_HALF;
    yaw = 2.0F * atan2f(z, w);
    roll = 0.0F;
  } else if (test < -(0.5F - EG_EPSILON_6)) {
    pitch = -EG_PI_HALF;
    yaw = -2.0F * atan2f(z, w);
    roll = 0.0F;
  } else {
    pitch = asinf(2.0F * test);
    yaw = atan2f(2.0F * ((w * y) - (x * z)), 1.0F - 2.0F * ((x * x) + (y * y)));
    roll = atan2f(2.0F * ((w * z) - (x * y)), 1.0F - 2.0F * ((y * y) + (z * z)));
  }

  eg_vector3_t q = {
    pitch,
    yaw,
    roll,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_from_euler_angles(eg_vector3_t a) {
  float pitch = eg_deg_to_rad(a.x);
  float yaw = eg_deg_to_rad(a.y);
  float roll = eg_deg_to_rad(a.z);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  eg_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_from_euler_angles_pyr(float p, float y, float r) {
  float pitch = eg_deg_to_rad(p);
  float yaw = eg_deg_to_rad(y);
  float roll = eg_deg_to_rad(r);

  float sp = sinf(pitch * 0.5F);
  float sy = sinf(yaw * 0.5F);
  float sr = sinf(roll * 0.5F);

  float cp = cosf(pitch * 0.5F);
  float cy = cosf(yaw * 0.5F);
  float cr = cosf(roll * 0.5F);

  eg_quaternion_t q = {
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy,
    cr * cp * cy + sr * sp * sy,
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_angle_axis(float a, eg_vector3_t b) {
  eg_vector3_t n = eg_vector3_norm(b);

  float a_half = a * 0.5F;
  float s = sinf(a_half);

  eg_quaternion_t q = {
    n.x * s,
    n.y * s,
    n.z * s,
    cosf(a_half),
  };

  return q;
}
__forceinline eg_quaternion_t eg_quaternion_norm(eg_quaternion_t a) {
  float l = eg_quaternion_length(a);

  if (l > 0.0F) {
    return eg_quaternion_muls(a, 1.0F / l);
  } else {
    return eg_quaternion_identity();
  }
}
__forceinline float eg_quaternion_dot(eg_quaternion_t a, eg_quaternion_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
__forceinline float eg_quaternion_length(eg_quaternion_t a) {
  return sqrtf(eg_quaternion_dot(a, a));
}
__forceinline float eg_quaternion_length2(eg_quaternion_t a) {
  return eg_quaternion_dot(a, a);
}
__forceinline void eg_quaternion_print(eg_quaternion_t a) {
  printf("[%f, %f, %f, %f]\n", a.x, a.y, a.z, a.w);
}
