#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include <math.h>
#include <stdio.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_vector3_t math_vector3_zero(void);
__forceinline math_vector3_t math_vector3_one(void);
__forceinline math_vector3_t math_vector3_right(void);
__forceinline math_vector3_t math_vector3_up(void);
__forceinline math_vector3_t math_vector3_front(void);
__forceinline math_vector3_t math_vector3_left(void);
__forceinline math_vector3_t math_vector3_down(void);
__forceinline math_vector3_t math_vector3_back(void);
__forceinline math_vector3_t math_vector3_xyz(float x, float y, float z);
__forceinline math_vector3_t math_vector3_negate(math_vector3_t a);
__forceinline math_vector3_t math_vector3_add(math_vector3_t a, math_vector3_t b);
__forceinline math_vector3_t math_vector3_sub(math_vector3_t a, math_vector3_t b);
__forceinline math_vector3_t math_vector3_mul(math_vector3_t a, math_vector3_t b);
__forceinline math_vector3_t math_vector3_div(math_vector3_t a, math_vector3_t b);
__forceinline math_vector3_t math_vector3_add_scalar(math_vector3_t a, float b);
__forceinline math_vector3_t math_vector3_sub_scalar(math_vector3_t a, float b);
__forceinline math_vector3_t math_vector3_mul_scalar(math_vector3_t a, float b);
__forceinline math_vector3_t math_vector3_div_scalar(math_vector3_t a, float b);
__forceinline math_vector3_t math_vector3_norm(math_vector3_t a);
__forceinline math_vector3_t math_vector3_cross(math_vector3_t a, math_vector3_t b);
__forceinline math_vector3_t math_vector3_rotate(math_vector3_t a, math_quaternion_t b);
__forceinline float math_vector3_dot(math_vector3_t a, math_vector3_t b);
__forceinline float math_vector3_length(math_vector3_t a);
__forceinline float math_vector3_length2(math_vector3_t a);
__forceinline void math_vector3_print(math_vector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/vector3.inl>

#endif // MATH_VECTOR3_H
