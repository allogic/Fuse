#ifndef EG_MATH_VECTOR3_H
#define EG_MATH_VECTOR3_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_vector3_t eg_vector3_zero(void);
__forceinline eg_vector3_t eg_vector3_one(void);
__forceinline eg_vector3_t eg_vector3_right(void);
__forceinline eg_vector3_t eg_vector3_up(void);
__forceinline eg_vector3_t eg_vector3_front(void);
__forceinline eg_vector3_t eg_vector3_left(void);
__forceinline eg_vector3_t eg_vector3_down(void);
__forceinline eg_vector3_t eg_vector3_back(void);
__forceinline eg_vector3_t eg_vector3_xyz(float x, float y, float z);
__forceinline eg_vector3_t eg_vector3_negate(eg_vector3_t a);
__forceinline eg_vector3_t eg_vector3_add(eg_vector3_t a, eg_vector3_t b);
__forceinline eg_vector3_t eg_vector3_sub(eg_vector3_t a, eg_vector3_t b);
__forceinline eg_vector3_t eg_vector3_mul(eg_vector3_t a, eg_vector3_t b);
__forceinline eg_vector3_t eg_vector3_div(eg_vector3_t a, eg_vector3_t b);
__forceinline eg_vector3_t eg_vector3_adds(eg_vector3_t a, float b);
__forceinline eg_vector3_t eg_vector3_subs(eg_vector3_t a, float b);
__forceinline eg_vector3_t eg_vector3_muls(eg_vector3_t a, float b);
__forceinline eg_vector3_t eg_vector3_divs(eg_vector3_t a, float b);
__forceinline eg_vector3_t eg_vector3_norm(eg_vector3_t a);
__forceinline eg_vector3_t eg_vector3_cross(eg_vector3_t a, eg_vector3_t b);
__forceinline eg_vector3_t eg_vector3_rotate(eg_vector3_t a, eg_quaternion_t b);
__forceinline float eg_vector3_dot(eg_vector3_t a, eg_vector3_t b);
__forceinline float eg_vector3_length(eg_vector3_t a);
__forceinline float eg_vector3_length2(eg_vector3_t a);
__forceinline void eg_vector3_print(eg_vector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_vector3.inl>

#endif // EG_MATH_VECTOR3_H
