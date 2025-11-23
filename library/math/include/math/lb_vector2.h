#ifndef LB_MATH_VECTOR2_H
#define LB_MATH_VECTOR2_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_vector2_t lb_vector2_zero(void);
__forceinline lb_vector2_t lb_vector2_one(void);
__forceinline lb_vector2_t lb_vector2_right(void);
__forceinline lb_vector2_t lb_vector2_up(void);
__forceinline lb_vector2_t lb_vector2_left(void);
__forceinline lb_vector2_t lb_vector2_down(void);
__forceinline lb_vector2_t lb_vector2_xy(float x, float y);
__forceinline lb_vector2_t lb_vector2_negate(lb_vector2_t a);
__forceinline lb_vector2_t lb_vector2_add(lb_vector2_t a, lb_vector2_t b);
__forceinline lb_vector2_t lb_vector2_sub(lb_vector2_t a, lb_vector2_t b);
__forceinline lb_vector2_t lb_vector2_mul(lb_vector2_t a, lb_vector2_t b);
__forceinline lb_vector2_t lb_vector2_div(lb_vector2_t a, lb_vector2_t b);
__forceinline lb_vector2_t lb_vector2_adds(lb_vector2_t a, float b);
__forceinline lb_vector2_t lb_vector2_subs(lb_vector2_t a, float b);
__forceinline lb_vector2_t lb_vector2_muls(lb_vector2_t a, float b);
__forceinline lb_vector2_t lb_vector2_divs(lb_vector2_t a, float b);
__forceinline lb_vector2_t lb_vector2_norm(lb_vector2_t a);
__forceinline float lb_vector2_dot(lb_vector2_t a, lb_vector2_t b);
__forceinline float lb_vector2_length(lb_vector2_t a);
__forceinline float lb_vector2_length2(lb_vector2_t a);
__forceinline void lb_vector2_print(lb_vector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_vector2.inl>

#endif // LB_MATH_VECTOR2_H
