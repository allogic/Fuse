#ifndef LB_MATH_MATRIX4_H
#define LB_MATH_MATRIX4_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_matrix4_t lb_matrix4_zero(void);
__forceinline lb_matrix4_t lb_matrix4_identity(void);
__forceinline lb_matrix4_t lb_matrix4_inverse(lb_matrix4_t a);
__forceinline lb_vector3_t lb_matrix4_position(lb_matrix4_t a);
__forceinline lb_quaternion_t lb_matrix4_rotation(lb_matrix4_t a);
__forceinline lb_vector3_t lb_matrix4_euler_angles(lb_matrix4_t a);
__forceinline lb_vector3_t lb_matrix4_scale(lb_matrix4_t a);
__forceinline lb_matrix4_t lb_matrix4_mul(lb_matrix4_t a, lb_matrix4_t b);
__forceinline lb_vector4_t lb_matrix4_mul_vector(lb_matrix4_t a, lb_vector4_t b);
__forceinline lb_matrix4_t lb_matrix4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline lb_matrix4_t lb_matrix4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline lb_matrix4_t lb_matrix4_look_at(lb_vector3_t eye, lb_vector3_t center, lb_vector3_t up);
__forceinline void lb_matrix4_decompose(lb_matrix4_t a, lb_vector3_t *p, lb_quaternion_t *r, lb_vector3_t *s);
__forceinline void lb_matrix4_print(lb_matrix4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_matrix4.inl>

#endif // LB_MATH_MATRIX4_H
