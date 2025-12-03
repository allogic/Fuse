#ifndef EG_MATH_IVECTOR2_H
#define EG_MATH_IVECTOR2_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_ivector2_t eg_ivector2_zero(void);
__forceinline eg_ivector2_t eg_ivector2_one(void);
__forceinline eg_ivector2_t eg_ivector2_right(void);
__forceinline eg_ivector2_t eg_ivector2_up(void);
__forceinline eg_ivector2_t eg_ivector2_left(void);
__forceinline eg_ivector2_t eg_ivector2_down(void);
__forceinline eg_ivector2_t eg_ivector2_xy(int32_t x, int32_t y);
__forceinline eg_ivector2_t eg_ivector2_negate(eg_ivector2_t a);
__forceinline eg_ivector2_t eg_ivector2_add(eg_ivector2_t a, eg_ivector2_t b);
__forceinline eg_ivector2_t eg_ivector2_sub(eg_ivector2_t a, eg_ivector2_t b);
__forceinline eg_ivector2_t eg_ivector2_mul(eg_ivector2_t a, eg_ivector2_t b);
__forceinline eg_ivector2_t eg_ivector2_div(eg_ivector2_t a, eg_ivector2_t b);
__forceinline eg_ivector2_t eg_ivector2_adds(eg_ivector2_t a, int32_t b);
__forceinline eg_ivector2_t eg_ivector2_subs(eg_ivector2_t a, int32_t b);
__forceinline eg_ivector2_t eg_ivector2_muls(eg_ivector2_t a, int32_t b);
__forceinline eg_ivector2_t eg_ivector2_divs(eg_ivector2_t a, int32_t b);
__forceinline float eg_ivector2_dot(eg_ivector2_t a, eg_ivector2_t b);
__forceinline float eg_ivector2_length(eg_ivector2_t a);
__forceinline float eg_ivector2_length2(eg_ivector2_t a);
__forceinline void eg_ivector2_print(eg_ivector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_ivector2.inl>

#endif // EG_MATH_IVECTOR2_H
