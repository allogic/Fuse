#ifndef LB_MATH_VECTOR3_H
#define LB_MATH_VECTOR3_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline lb_vector3_t lb_vector3_zero(void);
__forceinline lb_vector3_t lb_vector3_one(void);
__forceinline lb_vector3_t lb_vector3_right(void);
__forceinline lb_vector3_t lb_vector3_up(void);
__forceinline lb_vector3_t lb_vector3_front(void);
__forceinline lb_vector3_t lb_vector3_left(void);
__forceinline lb_vector3_t lb_vector3_down(void);
__forceinline lb_vector3_t lb_vector3_back(void);
__forceinline lb_vector3_t lb_vector3_xyz(float x, float y, float z);
__forceinline lb_vector3_t lb_vector3_negate(lb_vector3_t a);
__forceinline lb_vector3_t lb_vector3_add(lb_vector3_t a, lb_vector3_t b);
__forceinline lb_vector3_t lb_vector3_sub(lb_vector3_t a, lb_vector3_t b);
__forceinline lb_vector3_t lb_vector3_mul(lb_vector3_t a, lb_vector3_t b);
__forceinline lb_vector3_t lb_vector3_div(lb_vector3_t a, lb_vector3_t b);
__forceinline lb_vector3_t lb_vector3_adds(lb_vector3_t a, float b);
__forceinline lb_vector3_t lb_vector3_subs(lb_vector3_t a, float b);
__forceinline lb_vector3_t lb_vector3_muls(lb_vector3_t a, float b);
__forceinline lb_vector3_t lb_vector3_divs(lb_vector3_t a, float b);
__forceinline lb_vector3_t lb_vector3_norm(lb_vector3_t a);
__forceinline lb_vector3_t lb_vector3_cross(lb_vector3_t a, lb_vector3_t b);
__forceinline lb_vector3_t lb_vector3_rotate(lb_vector3_t a, lb_quaternion_t b);
__forceinline float lb_vector3_dot(lb_vector3_t a, lb_vector3_t b);
__forceinline float lb_vector3_length(lb_vector3_t a);
__forceinline float lb_vector3_length2(lb_vector3_t a);
__forceinline void lb_vector3_print(lb_vector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_vector3.inl>

#endif // LB_MATH_VECTOR3_H
