#ifndef LB_MATH_IVECTOR2_H
#define LB_MATH_IVECTOR2_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_ivector2_t lb_ivector2_zero(void);
__forceinline lb_ivector2_t lb_ivector2_one(void);
__forceinline lb_ivector2_t lb_ivector2_right(void);
__forceinline lb_ivector2_t lb_ivector2_up(void);
__forceinline lb_ivector2_t lb_ivector2_left(void);
__forceinline lb_ivector2_t lb_ivector2_down(void);
__forceinline lb_ivector2_t lb_ivector2_xy(int32_t x, int32_t y);
__forceinline lb_ivector2_t lb_ivector2_negate(lb_ivector2_t a);
__forceinline lb_ivector2_t lb_ivector2_add(lb_ivector2_t a, lb_ivector2_t b);
__forceinline lb_ivector2_t lb_ivector2_sub(lb_ivector2_t a, lb_ivector2_t b);
__forceinline lb_ivector2_t lb_ivector2_mul(lb_ivector2_t a, lb_ivector2_t b);
__forceinline lb_ivector2_t lb_ivector2_div(lb_ivector2_t a, lb_ivector2_t b);
__forceinline lb_ivector2_t lb_ivector2_adds(lb_ivector2_t a, int32_t b);
__forceinline lb_ivector2_t lb_ivector2_subs(lb_ivector2_t a, int32_t b);
__forceinline lb_ivector2_t lb_ivector2_muls(lb_ivector2_t a, int32_t b);
__forceinline lb_ivector2_t lb_ivector2_divs(lb_ivector2_t a, int32_t b);
__forceinline float lb_ivector2_dot(lb_ivector2_t a, lb_ivector2_t b);
__forceinline float lb_ivector2_length(lb_ivector2_t a);
__forceinline float lb_ivector2_length2(lb_ivector2_t a);
__forceinline void lb_ivector2_print(lb_ivector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_ivector2.inl>

#endif // LB_MATH_IVECTOR2_H
