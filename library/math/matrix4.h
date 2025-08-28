#ifndef MATH_MATRIX4_H
#define MATH_MATRIX4_H

#include <math.h>
#include <stdio.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline matrix4_t matrix4_zero(void);
__forceinline matrix4_t matrix4_identity(void);
__forceinline matrix4_t matrix4_inverse(matrix4_t a);
__forceinline vector3_t matrix4_position(matrix4_t a);
__forceinline quaternion_t matrix4_rotation(matrix4_t a);
__forceinline vector3_t matrix4_euler_angles(matrix4_t a);
__forceinline vector3_t matrix4_scale(matrix4_t a);
__forceinline matrix4_t matrix4_mul(matrix4_t a, matrix4_t b);
__forceinline vector4_t matrix4_mul_vector(matrix4_t a, vector4_t b);
__forceinline matrix4_t matrix4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline matrix4_t matrix4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline matrix4_t matrix4_look_at(vector3_t eye, vector3_t center, vector3_t up);
__forceinline void matrix4_decompose(matrix4_t a, vector3_t *p, quaternion_t *r, vector3_t *s);
__forceinline void matrix4_print(matrix4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/matrix4.inl>

#endif // MATH_MATRIX4_H
