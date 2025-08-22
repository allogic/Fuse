#ifndef MATH_MATRIX4_H
#define MATH_MATRIX4_H

#include <math.h>
#include <stdio.h>

#include <engine/math/constants.h>
#include <engine/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_matrix4_t math_matrix4_zero(void);
__forceinline math_matrix4_t math_matrix4_identity(void);
__forceinline math_matrix4_t math_matrix4_inverse(math_matrix4_t a);
__forceinline math_vector3_t math_matrix4_position(math_matrix4_t a);
__forceinline math_quaternion_t math_matrix4_rotation(math_matrix4_t a);
__forceinline math_vector3_t math_matrix4_euler_angles(math_matrix4_t a);
__forceinline math_vector3_t math_matrix4_scale(math_matrix4_t a);
__forceinline math_matrix4_t math_matrix4_mul(math_matrix4_t a, math_matrix4_t b);
__forceinline math_vector4_t math_matrix4_mul_vector(math_matrix4_t a, math_vector4_t b);
__forceinline math_matrix4_t math_matrix4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline math_matrix4_t math_matrix4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline math_matrix4_t math_matrix4_look_at(math_vector3_t eye, math_vector3_t center, math_vector3_t up);
__forceinline void math_matrix4_decompose(math_matrix4_t a, math_vector3_t *p, math_quaternion_t *r, math_vector3_t *s);
__forceinline void math_matrix4_print(math_matrix4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/matrix4.inl>

#endif // MATH_MATRIX4_H
