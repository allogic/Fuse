#ifndef MATH_UTILITY_H
#define MATH_UTILITY_H

#include <math.h>
#include <stdint.h>

#include <engine/math/constants.h>
#include <engine/math/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float math_utility_deg_to_rad(float a);
__forceinline float math_utility_rad_to_deg(float a);
__forceinline float math_utility_clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/utility.inl>

#endif // MATH_UTILITY_H
