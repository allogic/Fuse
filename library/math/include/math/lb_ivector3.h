#ifndef LB_MATH_IVECTOR3_H
#define LB_MATH_IVECTOR3_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_ivector3_t lb_ivector3_zero(void);
__forceinline lb_ivector3_t lb_ivector3_one(void);
__forceinline lb_ivector3_t lb_ivector3_right(void);
__forceinline lb_ivector3_t lb_ivector3_up(void);
__forceinline lb_ivector3_t lb_ivector3_front(void);
__forceinline lb_ivector3_t lb_ivector3_left(void);
__forceinline lb_ivector3_t lb_ivector3_down(void);
__forceinline lb_ivector3_t lb_ivector3_back(void);
__forceinline lb_ivector3_t lb_ivector3_xyz(int32_t x, int32_t y, int32_t z);
__forceinline lb_ivector3_t lb_ivector3_negate(lb_ivector3_t a);
__forceinline lb_ivector3_t lb_ivector3_add(lb_ivector3_t a, lb_ivector3_t b);
__forceinline lb_ivector3_t lb_ivector3_sub(lb_ivector3_t a, lb_ivector3_t b);
__forceinline lb_ivector3_t lb_ivector3_mul(lb_ivector3_t a, lb_ivector3_t b);
__forceinline lb_ivector3_t lb_ivector3_div(lb_ivector3_t a, lb_ivector3_t b);
__forceinline lb_ivector3_t lb_ivector3_adds(lb_ivector3_t a, int32_t b);
__forceinline lb_ivector3_t lb_ivector3_subs(lb_ivector3_t a, int32_t b);
__forceinline lb_ivector3_t lb_ivector3_muls(lb_ivector3_t a, int32_t b);
__forceinline lb_ivector3_t lb_ivector3_divs(lb_ivector3_t a, int32_t b);
__forceinline float lb_ivector3_dot(lb_ivector3_t a, lb_ivector3_t b);
__forceinline float lb_ivector3_length(lb_ivector3_t a);
__forceinline float lb_ivector3_length2(lb_ivector3_t a);
__forceinline void lb_ivector3_print(lb_ivector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_ivector3.inl>

#endif // LB_MATH_IVECTOR3_H
