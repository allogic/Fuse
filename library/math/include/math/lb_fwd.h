#ifndef LB_MATH_FWD_H
#define LB_MATH_FWD_H

typedef struct lb_vector2_t {
  float x;
  float y;
} lb_vector2_t;

typedef struct lb_ivector2_t {
  int32_t x;
  int32_t y;
} lb_ivector2_t;

typedef struct lb_vector3_t {
  float x;
  float y;
  float z;
} lb_vector3_t;

typedef struct lb_ivector3_t {
  int32_t x;
  int32_t y;
  int32_t z;
} lb_ivector3_t;

typedef struct lb_vector4_t {
  float x;
  float y;
  float z;
  float w;
} lb_vector4_t;

typedef struct lb_ivector4_t {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} lb_ivector4_t;

typedef struct lb_quaternion_t {
  float x;
  float y;
  float z;
  float w;
} lb_quaternion_t;

typedef struct lb_matrix4_t {
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
} lb_matrix4_t;

#endif // LB_MATH_FWD_H
