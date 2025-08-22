#ifndef MATH_VECTOR4_H
#define MATH_VECTOR4_H

#include <math.h>
#include <stdio.h>

#include <engine/math/constants.h>
#include <engine/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_vector4_t math_vector4_zero(void);
__forceinline math_vector4_t math_vector4_one(void);
__forceinline math_vector4_t math_vector4_xyzw(float x, float y, float z, float w);
__forceinline math_vector4_t math_vector4_negate(math_vector4_t a);
__forceinline math_vector4_t math_vector4_add(math_vector4_t a, math_vector4_t b);
__forceinline math_vector4_t math_vector4_sub(math_vector4_t a, math_vector4_t b);
__forceinline math_vector4_t math_vector4_mul(math_vector4_t a, math_vector4_t b);
__forceinline math_vector4_t math_vector4_div(math_vector4_t a, math_vector4_t b);
__forceinline math_vector4_t math_vector4_add_scalar(math_vector4_t a, float b);
__forceinline math_vector4_t math_vector4_sub_scalar(math_vector4_t a, float b);
__forceinline math_vector4_t math_vector4_mul_scalar(math_vector4_t a, float b);
__forceinline math_vector4_t math_vector4_div_scalar(math_vector4_t a, float b);
__forceinline math_vector4_t math_vector4_norm(math_vector4_t a);
__forceinline float math_vector4_dot(math_vector4_t a, math_vector4_t b);
__forceinline float math_vector4_length(math_vector4_t a);
__forceinline float math_vector4_length2(math_vector4_t a);
__forceinline void math_vector4_print(math_vector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/vector4.inl>

#endif // MATH_VECTOR4_H
