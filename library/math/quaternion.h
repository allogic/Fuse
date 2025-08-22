#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include <math.h>
#include <stdio.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_quaternion_t math_quaternion_zero(void);
__forceinline math_quaternion_t math_quaternion_identity(void);
__forceinline math_quaternion_t math_quaternion_xyzw(float x, float y, float z, float w);
__forceinline math_quaternion_t math_quaternion_mul(math_quaternion_t a, math_quaternion_t b);
__forceinline math_quaternion_t math_quaternion_mul_scalar(math_quaternion_t a, float b);
__forceinline math_quaternion_t math_quaternion_conjugate(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_right(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_up(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_front(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_left(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_down(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_back(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_to_euler_angles(math_quaternion_t a);
__forceinline math_vector3_t math_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w);
__forceinline math_quaternion_t math_quaternion_from_euler_angles(math_vector3_t a);
__forceinline math_quaternion_t math_quaternion_from_euler_angles_pyr(float p, float y, float r);
__forceinline math_quaternion_t math_quaternion_angle_axis(float a, math_vector3_t b);
__forceinline math_quaternion_t math_quaternion_norm(math_quaternion_t a);
__forceinline float math_quaternion_dot(math_quaternion_t a, math_quaternion_t b);
__forceinline float math_quaternion_length(math_quaternion_t a);
__forceinline float math_quaternion_length2(math_quaternion_t a);
__forceinline void math_quaternion_print(math_quaternion_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/quaternion.inl>

#endif // MATH_QUATERNION_H
