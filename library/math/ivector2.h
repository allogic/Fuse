#ifndef MATH_IVECTOR2_H
#define MATH_IVECTOR2_H

#include <math.h>
#include <stdio.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_ivector2_t math_ivector2_zero(void);
__forceinline math_ivector2_t math_ivector2_one(void);
__forceinline math_ivector2_t math_ivector2_right(void);
__forceinline math_ivector2_t math_ivector2_up(void);
__forceinline math_ivector2_t math_ivector2_left(void);
__forceinline math_ivector2_t math_ivector2_down(void);
__forceinline math_ivector2_t math_ivector2_xy(int32_t x, int32_t y);
__forceinline math_ivector2_t math_ivector2_negate(math_ivector2_t a);
__forceinline math_ivector2_t math_ivector2_add(math_ivector2_t a, math_ivector2_t b);
__forceinline math_ivector2_t math_ivector2_sub(math_ivector2_t a, math_ivector2_t b);
__forceinline math_ivector2_t math_ivector2_mul(math_ivector2_t a, math_ivector2_t b);
__forceinline math_ivector2_t math_ivector2_div(math_ivector2_t a, math_ivector2_t b);
__forceinline math_ivector2_t math_ivector2_add_scalar(math_ivector2_t a, int32_t b);
__forceinline math_ivector2_t math_ivector2_sub_scalar(math_ivector2_t a, int32_t b);
__forceinline math_ivector2_t math_ivector2_mul_scalar(math_ivector2_t a, int32_t b);
__forceinline math_ivector2_t math_ivector2_div_scalar(math_ivector2_t a, int32_t b);
__forceinline float math_ivector2_dot(math_ivector2_t a, math_ivector2_t b);
__forceinline float math_ivector2_length(math_ivector2_t a);
__forceinline float math_ivector2_length2(math_ivector2_t a);
__forceinline void math_ivector2_print(math_ivector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/ivector2.inl>

#endif // MATH_IVECTOR2_H
