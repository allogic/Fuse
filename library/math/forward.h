#ifndef MATH_FORWARD_H
#define MATH_FORWARD_H

#include <stdint.h>

typedef struct math_vector2_t {
  float x;
  float y;
} math_vector2_t;

typedef struct math_ivector2_t {
  int32_t x;
  int32_t y;
} math_ivector2_t;

typedef struct math_vector3_t {
  float x;
  float y;
  float z;
} math_vector3_t;

typedef struct math_ivector3_t {
  int32_t x;
  int32_t y;
  int32_t z;
} math_ivector3_t;

typedef struct math_vector4_t {
  float x;
  float y;
  float z;
  float w;
} math_vector4_t;

typedef struct math_ivector4_t {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} math_ivector4_t;

typedef struct math_quaternion_t {
  float x;
  float y;
  float z;
  float w;
} math_quaternion_t;

typedef struct math_matrix4_t {
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
} math_matrix4_t;

#endif // MATH_FORWARD_H
