#ifndef MATH_FORWARD_H
#define MATH_FORWARD_H

#include <stdint.h>

typedef struct vector2_t {
  float x;
  float y;
} vector2_t;

typedef struct ivector2_t {
  int32_t x;
  int32_t y;
} ivector2_t;

typedef struct vector3_t {
  float x;
  float y;
  float z;
} vector3_t;

typedef struct ivector3_t {
  int32_t x;
  int32_t y;
  int32_t z;
} ivector3_t;

typedef struct vector4_t {
  float x;
  float y;
  float z;
  float w;
} vector4_t;

typedef struct ivector4_t {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} ivector4_t;

typedef struct quaternion_t {
  float x;
  float y;
  float z;
  float w;
} quaternion_t;

typedef struct matrix4_t {
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;
} matrix4_t;

#endif // MATH_FORWARD_H
