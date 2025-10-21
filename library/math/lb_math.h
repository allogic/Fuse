#ifndef LB_MATH_MATH_H
#define LB_MATH_MATH_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float deg_to_rad(float a);
__forceinline float rad_to_deg(float a);

__forceinline float clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/lb_math.inl>

#endif // LB_MATH_MATH_H
