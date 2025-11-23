#ifndef LB_MATH_VECTOR4_H
#define LB_MATH_VECTOR4_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_vector4_t lb_vector4_zero(void);
__forceinline lb_vector4_t lb_vector4_one(void);
__forceinline lb_vector4_t lb_vector4_xyzw(float x, float y, float z, float w);
__forceinline lb_vector4_t lb_vector4_negate(lb_vector4_t a);
__forceinline lb_vector4_t lb_vector4_add(lb_vector4_t a, lb_vector4_t b);
__forceinline lb_vector4_t lb_vector4_sub(lb_vector4_t a, lb_vector4_t b);
__forceinline lb_vector4_t lb_vector4_mul(lb_vector4_t a, lb_vector4_t b);
__forceinline lb_vector4_t lb_vector4_div(lb_vector4_t a, lb_vector4_t b);
__forceinline lb_vector4_t lb_vector4_adds(lb_vector4_t a, float b);
__forceinline lb_vector4_t lb_vector4_subs(lb_vector4_t a, float b);
__forceinline lb_vector4_t lb_vector4_muls(lb_vector4_t a, float b);
__forceinline lb_vector4_t lb_vector4_divs(lb_vector4_t a, float b);
__forceinline lb_vector4_t lb_vector4_norm(lb_vector4_t a);
__forceinline float lb_vector4_dot(lb_vector4_t a, lb_vector4_t b);
__forceinline float lb_vector4_length(lb_vector4_t a);
__forceinline float lb_vector4_length2(lb_vector4_t a);
__forceinline void lb_vector4_print(lb_vector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_vector4.inl>

#endif // LB_MATH_VECTOR4_H
