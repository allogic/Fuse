#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdint.h>

#include <library/math/constants.h>
#include <library/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float math_deg_to_rad(float a);
__forceinline float math_rad_to_deg(float a);

__forceinline float math_clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/math.inl>

#endif // MATH_H
