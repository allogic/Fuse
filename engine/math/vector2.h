#ifndef MATH_VECTOR2_H
#define MATH_VECTOR2_H

#include <math.h>
#include <stdio.h>

#include <engine/math/constants.h>
#include <engine/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_vector2_t math_vector2_zero(void);
__forceinline math_vector2_t math_vector2_one(void);
__forceinline math_vector2_t math_vector2_right(void);
__forceinline math_vector2_t math_vector2_up(void);
__forceinline math_vector2_t math_vector2_left(void);
__forceinline math_vector2_t math_vector2_down(void);
__forceinline math_vector2_t math_vector2_xy(float x, float y);
__forceinline math_vector2_t math_vector2_negate(math_vector2_t a);
__forceinline math_vector2_t math_vector2_add(math_vector2_t a, math_vector2_t b);
__forceinline math_vector2_t math_vector2_sub(math_vector2_t a, math_vector2_t b);
__forceinline math_vector2_t math_vector2_mul(math_vector2_t a, math_vector2_t b);
__forceinline math_vector2_t math_vector2_div(math_vector2_t a, math_vector2_t b);
__forceinline math_vector2_t math_vector2_add_scalar(math_vector2_t a, float b);
__forceinline math_vector2_t math_vector2_sub_scalar(math_vector2_t a, float b);
__forceinline math_vector2_t math_vector2_mul_scalar(math_vector2_t a, float b);
__forceinline math_vector2_t math_vector2_div_scalar(math_vector2_t a, float b);
__forceinline math_vector2_t math_vector2_norm(math_vector2_t a);
__forceinline float math_vector2_dot(math_vector2_t a, math_vector2_t b);
__forceinline float math_vector2_length(math_vector2_t a);
__forceinline float math_vector2_length2(math_vector2_t a);
__forceinline void math_vector2_print(math_vector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/vector2.inl>

#endif // MATH_VECTOR2_H
