#ifndef LB_MATH_IVECTOR4_H
#define LB_MATH_IVECTOR4_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_ivector4_t lb_ivector4_zero(void);
__forceinline lb_ivector4_t lb_ivector4_one(void);
__forceinline lb_ivector4_t lb_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w);
__forceinline lb_ivector4_t lb_ivector4_negate(lb_ivector4_t a);
__forceinline lb_ivector4_t lb_ivector4_add(lb_ivector4_t a, lb_ivector4_t b);
__forceinline lb_ivector4_t lb_ivector4_sub(lb_ivector4_t a, lb_ivector4_t b);
__forceinline lb_ivector4_t lb_ivector4_mul(lb_ivector4_t a, lb_ivector4_t b);
__forceinline lb_ivector4_t lb_ivector4_div(lb_ivector4_t a, lb_ivector4_t b);
__forceinline lb_ivector4_t lb_ivector4_adds(lb_ivector4_t a, int32_t b);
__forceinline lb_ivector4_t lb_ivector4_subs(lb_ivector4_t a, int32_t b);
__forceinline lb_ivector4_t lb_ivector4_muls(lb_ivector4_t a, int32_t b);
__forceinline lb_ivector4_t lb_ivector4_divs(lb_ivector4_t a, int32_t b);
__forceinline float lb_ivector4_dot(lb_ivector4_t a, lb_ivector4_t b);
__forceinline float lb_ivector4_length(lb_ivector4_t a);
__forceinline float lb_ivector4_length2(lb_ivector4_t a);
__forceinline void lb_ivector4_print(lb_ivector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_ivector4.inl>

#endif // LB_MATH_IVECTOR4_H
