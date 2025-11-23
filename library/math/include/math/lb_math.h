#ifndef LB_MATH_MATH_H
#define LB_MATH_MATH_H

#include <math/lb_pch.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float lb_deg_to_rad(float a);
__forceinline float lb_rad_to_deg(float a);

__forceinline float lb_clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <math/lb_math.inl>

#endif // LB_MATH_MATH_H
