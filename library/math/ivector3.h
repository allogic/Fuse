#ifndef MATH_IVECTOR3_H
#define MATH_IVECTOR3_H

#include <math.h>
#include <stdio.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline math_ivector3_t math_ivector3_zero(void);
__forceinline math_ivector3_t math_ivector3_one(void);
__forceinline math_ivector3_t math_ivector3_right(void);
__forceinline math_ivector3_t math_ivector3_up(void);
__forceinline math_ivector3_t math_ivector3_front(void);
__forceinline math_ivector3_t math_ivector3_left(void);
__forceinline math_ivector3_t math_ivector3_down(void);
__forceinline math_ivector3_t math_ivector3_back(void);
__forceinline math_ivector3_t math_ivector3_xyz(int32_t x, int32_t y, int32_t z);
__forceinline math_ivector3_t math_ivector3_negate(math_ivector3_t a);
__forceinline math_ivector3_t math_ivector3_add(math_ivector3_t a, math_ivector3_t b);
__forceinline math_ivector3_t math_ivector3_sub(math_ivector3_t a, math_ivector3_t b);
__forceinline math_ivector3_t math_ivector3_mul(math_ivector3_t a, math_ivector3_t b);
__forceinline math_ivector3_t math_ivector3_div(math_ivector3_t a, math_ivector3_t b);
__forceinline math_ivector3_t math_ivector3_add_scalar(math_ivector3_t a, int32_t b);
__forceinline math_ivector3_t math_ivector3_sub_scalar(math_ivector3_t a, int32_t b);
__forceinline math_ivector3_t math_ivector3_mul_scalar(math_ivector3_t a, int32_t b);
__forceinline math_ivector3_t math_ivector3_div_scalar(math_ivector3_t a, int32_t b);
__forceinline float math_ivector3_dot(math_ivector3_t a, math_ivector3_t b);
__forceinline float math_ivector3_length(math_ivector3_t a);
__forceinline float math_ivector3_length2(math_ivector3_t a);
__forceinline void math_ivector3_print(math_ivector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/ivector3.inl>

#endif // MATH_IVECTOR3_H
