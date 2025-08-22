#ifndef MATH_IVECTOR4_H
#define MATH_IVECTOR4_H

#include <math.h>
#include <stdio.h>

#include <engine/math/constants.h>
#include <engine/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_ivector4_t math_ivector4_zero(void);
__forceinline math_ivector4_t math_ivector4_one(void);
__forceinline math_ivector4_t math_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w);
__forceinline math_ivector4_t math_ivector4_negate(math_ivector4_t a);
__forceinline math_ivector4_t math_ivector4_add(math_ivector4_t a, math_ivector4_t b);
__forceinline math_ivector4_t math_ivector4_sub(math_ivector4_t a, math_ivector4_t b);
__forceinline math_ivector4_t math_ivector4_mul(math_ivector4_t a, math_ivector4_t b);
__forceinline math_ivector4_t math_ivector4_div(math_ivector4_t a, math_ivector4_t b);
__forceinline math_ivector4_t math_ivector4_add_scalar(math_ivector4_t a, int32_t b);
__forceinline math_ivector4_t math_ivector4_sub_scalar(math_ivector4_t a, int32_t b);
__forceinline math_ivector4_t math_ivector4_mul_scalar(math_ivector4_t a, int32_t b);
__forceinline math_ivector4_t math_ivector4_div_scalar(math_ivector4_t a, int32_t b);
__forceinline float math_ivector4_dot(math_ivector4_t a, math_ivector4_t b);
__forceinline float math_ivector4_length(math_ivector4_t a);
__forceinline float math_ivector4_length2(math_ivector4_t a);
__forceinline void math_ivector4_print(math_ivector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/ivector4.inl>

#endif // MATH_IVECTOR4_H
