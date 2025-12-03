#ifndef EG_MATH_VECTOR4_H
#define EG_MATH_VECTOR4_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_vector4_t eg_vector4_zero(void);
__forceinline eg_vector4_t eg_vector4_one(void);
__forceinline eg_vector4_t eg_vector4_xyzw(float x, float y, float z, float w);
__forceinline eg_vector4_t eg_vector4_negate(eg_vector4_t a);
__forceinline eg_vector4_t eg_vector4_add(eg_vector4_t a, eg_vector4_t b);
__forceinline eg_vector4_t eg_vector4_sub(eg_vector4_t a, eg_vector4_t b);
__forceinline eg_vector4_t eg_vector4_mul(eg_vector4_t a, eg_vector4_t b);
__forceinline eg_vector4_t eg_vector4_div(eg_vector4_t a, eg_vector4_t b);
__forceinline eg_vector4_t eg_vector4_adds(eg_vector4_t a, float b);
__forceinline eg_vector4_t eg_vector4_subs(eg_vector4_t a, float b);
__forceinline eg_vector4_t eg_vector4_muls(eg_vector4_t a, float b);
__forceinline eg_vector4_t eg_vector4_divs(eg_vector4_t a, float b);
__forceinline eg_vector4_t eg_vector4_norm(eg_vector4_t a);
__forceinline float eg_vector4_dot(eg_vector4_t a, eg_vector4_t b);
__forceinline float eg_vector4_length(eg_vector4_t a);
__forceinline float eg_vector4_length2(eg_vector4_t a);
__forceinline void eg_vector4_print(eg_vector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_vector4.inl>

#endif // EG_MATH_VECTOR4_H
