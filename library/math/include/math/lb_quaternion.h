#ifndef LB_MATH_QUATERNION_H
#define LB_MATH_QUATERNION_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_quaternion_t lb_quaternion_zero(void);
__forceinline lb_quaternion_t lb_quaternion_identity(void);
__forceinline lb_quaternion_t lb_quaternion_xyzw(float x, float y, float z, float w);
__forceinline lb_quaternion_t lb_quaternion_add(lb_quaternion_t a, lb_quaternion_t b);
__forceinline lb_quaternion_t lb_quaternion_adds(lb_quaternion_t a, float b);
__forceinline lb_quaternion_t lb_quaternion_mul(lb_quaternion_t a, lb_quaternion_t b);
__forceinline lb_quaternion_t lb_quaternion_muls(lb_quaternion_t a, float b);
__forceinline lb_quaternion_t lb_quaternion_conjugate(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_right(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_up(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_front(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_left(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_down(lb_quaternion_t a);
__forceinline lb_vector3_t lb_quaternion_back(lb_quaternion_t a);
__forceinline lb_vector3_t lb_lb_quaternion_to_euler_angles(lb_quaternion_t a);
__forceinline lb_vector3_t lb_lb_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w);
__forceinline lb_quaternion_t lb_quaternion_from_euler_angles(lb_vector3_t a);
__forceinline lb_quaternion_t lb_quaternion_from_euler_angles_pyr(float p, float y, float r);
__forceinline lb_quaternion_t lb_quaternion_angle_axis(float a, lb_vector3_t b);
__forceinline lb_quaternion_t lb_quaternion_norm(lb_quaternion_t a);
__forceinline float lb_quaternion_dot(lb_quaternion_t a, lb_quaternion_t b);
__forceinline float lb_quaternion_length(lb_quaternion_t a);
__forceinline float lb_quaternion_length2(lb_quaternion_t a);
__forceinline void lb_quaternion_print(lb_quaternion_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_quaternion.inl>

#endif // LB_MATH_QUATERNION_H
