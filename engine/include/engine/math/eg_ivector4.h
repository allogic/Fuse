#ifndef EG_MATH_IVECTOR4_H
#define EG_MATH_IVECTOR4_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_ivector4_t eg_ivector4_zero(void);
__forceinline eg_ivector4_t eg_ivector4_one(void);
__forceinline eg_ivector4_t eg_ivector4_xyzw(int32_t x, int32_t y, int32_t z, int32_t w);
__forceinline eg_ivector4_t eg_ivector4_negate(eg_ivector4_t a);
__forceinline eg_ivector4_t eg_ivector4_add(eg_ivector4_t a, eg_ivector4_t b);
__forceinline eg_ivector4_t eg_ivector4_sub(eg_ivector4_t a, eg_ivector4_t b);
__forceinline eg_ivector4_t eg_ivector4_mul(eg_ivector4_t a, eg_ivector4_t b);
__forceinline eg_ivector4_t eg_ivector4_div(eg_ivector4_t a, eg_ivector4_t b);
__forceinline eg_ivector4_t eg_ivector4_adds(eg_ivector4_t a, int32_t b);
__forceinline eg_ivector4_t eg_ivector4_subs(eg_ivector4_t a, int32_t b);
__forceinline eg_ivector4_t eg_ivector4_muls(eg_ivector4_t a, int32_t b);
__forceinline eg_ivector4_t eg_ivector4_divs(eg_ivector4_t a, int32_t b);
__forceinline float eg_ivector4_dot(eg_ivector4_t a, eg_ivector4_t b);
__forceinline float eg_ivector4_length(eg_ivector4_t a);
__forceinline float eg_ivector4_length2(eg_ivector4_t a);
__forceinline void eg_ivector4_print(eg_ivector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_ivector4.inl>

#endif // EG_MATH_IVECTOR4_H
