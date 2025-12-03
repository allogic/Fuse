#ifndef EG_MATH_MATH_H
#define EG_MATH_MATH_H

#define EG_PI_HALF ((float)1.57079632679489655800)
#define EG_PI ((float)3.14159265358979323846)
#define EG_TAU ((float)6.28318530717958623200)

#define EG_DEG_TO_RAD ((float)0.01745329251994329547)
#define EG_RAD_TO_DEG ((float)57.29577951308232286465)

#define EG_EPSILON_3 ((float)1.0E-3)
#define EG_EPSILON_4 ((float)1.0E-4)
#define EG_EPSILON_5 ((float)1.0E-5)
#define EG_EPSILON_6 ((float)1.0E-6)

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline float eg_deg_to_rad(float a);
__forceinline float eg_rad_to_deg(float a);

__forceinline float eg_clamp(float a, float min, float max);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_math.inl>

#endif // EG_MATH_MATH_H
