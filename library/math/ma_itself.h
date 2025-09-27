#ifndef MA_ITSELF_H
#define MA_ITSELF_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float math_deg_to_rad(float a);
__forceinline float math_rad_to_deg(float a);

__forceinline float math_clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/ma_itself.inl>

#endif // MA_ITSELF_H
